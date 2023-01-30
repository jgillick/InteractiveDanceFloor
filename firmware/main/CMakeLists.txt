idf_build_get_property(project_dir PROJECT_DIR)

idf_component_register(
  SRCS "main.c" "wifi.c" "websocket.c" "ota_update.c"
  INCLUDE_DIRS "."
  EMBED_TXTFILES ${project_dir}/server_certs/host_cert.pem
)
