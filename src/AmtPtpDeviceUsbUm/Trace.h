// Trace.h: Local type definitions for tracing

//
// Device Interface GUID
// 4a5064e5-7d39-41d1-a0e4-81097edce967
//
DEFINE_GUID(GUID_DEVINTERFACE_AmtPtpDevice,
	0x4a5064e5, 0x7d39, 0x41d1, 0xa0, 0xe4, 0x81, 0x09, 0x7e, 0xdc, 0xe9, 0x67);

//
// Define the tracing flags.
//
// Tracing GUID - efc3ce99-43ff-4b59-afe4-c856e1afd8b0
//

#define WPP_CONTROL_GUIDS													\
    WPP_DEFINE_CONTROL_GUID(												\
        AmtPtpDriverTraceGuid, (efc3ce99,43ff,4b59,afe4,c856e1afd8b0),		\
																			\
        WPP_DEFINE_BIT(AMTPTPDRIVER_ALL_INFO)								\
        WPP_DEFINE_BIT(TRACE_DRIVER)										\
        WPP_DEFINE_BIT(TRACE_DEVICE)										\
        WPP_DEFINE_BIT(TRACE_QUEUE)											\
		WPP_DEFINE_BIT(TRACE_INPUT)											\
        )                             

#define WPP_FLAG_LEVEL_LOGGER(flag, level)                                  \
    WPP_LEVEL_LOGGER(flag)

#define WPP_FLAG_LEVEL_ENABLED(flag, level)                                 \
    (WPP_LEVEL_ENABLED(flag) &&                                             \
     WPP_CONTROL(WPP_BIT_ ## flag).Level >= level)

#define WPP_LEVEL_FLAGS_LOGGER(lvl,flags) \
           WPP_LEVEL_LOGGER(flags)

#define WPP_LEVEL_FLAGS_ENABLED(lvl, flags) \
           (WPP_LEVEL_ENABLED(flags) && WPP_CONTROL(WPP_BIT_ ## flags).Level >= lvl)

//
// This comment block is scanned by the trace preprocessor to define our
// Trace function.
//
// begin_wpp config
// FUNC Trace{FLAG=MYDRIVER_ALL_INFO}(LEVEL, MSG, ...);
// FUNC TraceEvents(LEVEL, FLAGS, MSG, ...);
// end_wpp
//