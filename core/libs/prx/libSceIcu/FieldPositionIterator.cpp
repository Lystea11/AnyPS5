#include "prx/libSceIcu/FieldPositionIterator.hpp"
#include "prx/libSceIcu/RecoveredSignatures.hpp"
#include "prx/libSceIcu/Status.hpp"
#include "prx/libc/include/General.hpp"
#include "unicode/ufieldpositer.h"
#include <cstdint>
#include <memory>
#include <mutex>
#include <new>
#include <unordered_set>

namespace {

std::mutex iteratorMutex;
std::unordered_set<UFieldPositionIterator*> iterators;

}

std::unique_lock<std::mutex> Icu::LockFieldPositionIterator_nid_no_patch(UFieldPositionIterator* iterator, const char* caller) {
    std::unique_lock lock(iteratorMutex);
    if (!iterators.contains(iterator)) NotImplemented_nid_no_patch(caller);
    return lock;
}

extern "C" {

void APS5_VABI ufieldpositer_close_nid_postfix(UFieldPositionIterator* fpositer) {
    if (!fpositer) return;
    const std::lock_guard lock(iteratorMutex);
    if (!iterators.erase(fpositer)) NotImplemented_nid_no_patch(__func__);
    ufieldpositer_close(fpositer);
}

UFieldPositionIterator* APS5_VABI ufieldpositer_open_nid_postfix(std::int32_t* status) {
    if (Icu::Failure(status)) return nullptr;
    auto error = static_cast<UErrorCode>(*status);
    std::unique_ptr<UFieldPositionIterator, decltype(&ufieldpositer_close)> iterator(ufieldpositer_open(&error),
                                                                                 &ufieldpositer_close);
    *status = error;
    if (!iterator) return nullptr;
    try {
        const std::lock_guard lock(iteratorMutex);
        iterators.insert(iterator.get());
    } catch (const std::bad_alloc&) {
        *status = U_MEMORY_ALLOCATION_ERROR;
        return nullptr;
    }
    return iterator.release();
}

std::int32_t APS5_VABI ufieldpositer_next_nid_postfix(UFieldPositionIterator* fpositer, std::int32_t* beginIndex,
                                                    std::int32_t* endIndex) {
    const auto lock = Icu::LockFieldPositionIterator_nid_no_patch(fpositer, __func__);
    return ufieldpositer_next(fpositer, beginIndex, endIndex);
}

}
