idf_component_register(
    SRCS "src/firebase.c"
    INCLUDE_DIRS "include"
    REQUIRES esp_http_client esp-tls
)

