# CMake generated Testfile for 
# Source directory: D:/PersonalProject/RTOS_Scheduler/RTOS Scheduler
# Build directory: D:/PersonalProject/RTOS_Scheduler/RTOS Scheduler/out/build/x64-Debug
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[RTOSScheduler.BasicTests]=] "D:/PersonalProject/RTOS_Scheduler/RTOS Scheduler/out/build/x64-Debug/RTOSScheduler_tests.exe")
set_tests_properties([=[RTOSScheduler.BasicTests]=] PROPERTIES  _BACKTRACE_TRIPLES "D:/PersonalProject/RTOS_Scheduler/RTOS Scheduler/CMakeLists.txt;97;add_test;D:/PersonalProject/RTOS_Scheduler/RTOS Scheduler/CMakeLists.txt;0;")
subdirs("Extern/glfw")
