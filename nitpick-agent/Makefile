NPKC = /home/randy/Workspace/REPOS/nitpick/build/npkc
CFLAGS = -O0 -g
PKG_DIR = ../../nitpick-packages/packages

INCLUDES = -I src -I $(PKG_DIR)/nitpick-http/src -I $(PKG_DIR)/nitpick-json/src -I $(PKG_DIR)/nitpick-str/src -I $(PKG_DIR)/nitpick-vector/src -I $(PKG_DIR)/nitpick-args/src

LIBS = -Lshim -lworkspace_shim -lbus_shim -L$(PKG_DIR)/nitpick-http/shim -lnitpick_http_shim -L$(PKG_DIR)/nitpick-json/shim -lnitpick_json_shim -L$(PKG_DIR)/nitpick-vector/shim -lnitpick_vector_shim -lm -lcurl

all: nitpick-agent

nitpick-agent: src/main.npk src/ollama_client.npk src/workspace.npk src/distiller.npk src/bus.npk
	$(NPKC) $(CFLAGS) $(INCLUDES) src/main.npk -o nitpick-agent $(LIBS)

clean:
	rm -f nitpick-agent
