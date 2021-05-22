#if defined(_APP_DEBUG_ON_)

#define _APP_DEBUG_(type, text) \
  Serial.print(F("("));         \
  Serial.print(millis());       \
  Serial.print(F(" millis)"));  \
  Serial.print(F(" ["));        \
  Serial.print(type);           \
  Serial.print(F("] "));        \
  Serial.println(text);         \

#define _APP_DEBUG_VALUE_(type, text, value) \
  Serial.print(F("("));                      \
  Serial.print(millis());                    \
  Serial.print(F(" millis)"));               \
  Serial.print(F(" ["));                     \
  Serial.print(type);                        \
  Serial.print(F("] "));                     \
  Serial.print(text);                        \
  Serial.println(value);                     \

#else

#define _APP_DEBUG_(type, text) void();
#define _APP_DEBUG_VALUE_(type, text, value) void();

#endif