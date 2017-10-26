const char* g_pchMulticommonVersion = "1.5 (2017-07-26)";

/*
/////////////////////////////////////////////////////////////////////////////////////////////////////
1.0 (2016-11-24)
1) common和multicommon合并后的第一个版本
1.1 (2017-01-19)
1) Add some convenient output buffer/packet classes;
2) Add a common task queue class.
1.2 (2017-03-09)
1) 添加条件变量类
2) 添加CountDown计数类
3) lock类添加获取原生pthread_mutex_t接口
1.3 (2017-03-09)
1) Make CTimerTaskQueue event-driven instead of busy checking;
2) Let the log macros use global Log() function.
1.4 (2017-04-06)
1) Make the number of digits for display configurable in marketcode.xml instead of hard-coded.
1.5 (2017-07-26)
1) Log to stdout before logging is configured;
2) Add a method to get log file path.
*/
