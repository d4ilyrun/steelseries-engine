ADD_LIBRARY(
    device STATIC
    rival310.c
)

TARGET_LINK_LIBRARIES(device PRIVATE log)
