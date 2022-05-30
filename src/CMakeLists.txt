ADD_LIBRARY(
    driver STATIC
    device/rival310.c
)

TARGET_LINK_LIBRARIES(driver PRIVATE hidapi-hidraw)
