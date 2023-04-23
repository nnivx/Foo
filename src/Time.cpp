#include <Foo/Time.h>

#include <windows.h>

namespace {

    /** The following is from SFML Clock implementation */
    LARGE_INTEGER getFrequency() {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return frequency;
    }

}

namespace foo {

    Time Time::now() {
        /** The following is from SFML Clock implementation */
        // Force the following code to run on first core
        // (see http://msdn.microsoft.com/en-us/library/windows/desktop/ms644904(v=vs.85).aspx)
        HANDLE currentThread = GetCurrentThread();
        DWORD_PTR previousMask = SetThreadAffinityMask(currentThread, 1);

        // Get the frequency of the performance counter
        // (it is constant across the program lifetime)
        static LARGE_INTEGER frequency = getFrequency();

        // Get the current time
        LARGE_INTEGER BigTime;
        QueryPerformanceCounter(&BigTime);

        // Restore the thread affinity
        SetThreadAffinityMask(currentThread, previousMask);

        // Return the current time as microseconds
        return static_cast<int64_t>(1000000 * BigTime.QuadPart / frequency.QuadPart);
    }

}