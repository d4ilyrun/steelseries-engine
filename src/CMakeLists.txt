ADD_LIBRARY(log STATIC log.c)

ADD_SUBDIRECTORY(device)

ADD_LIBRARY(
    driver STATIC
    tmp.c
)

TARGET_LINK_LIBRARIES(driver PRIVATE
    hidapi-hidraw
    device
    log
)
