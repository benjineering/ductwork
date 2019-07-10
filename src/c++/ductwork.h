#include <string>
#include "../ductwork.h"

namespace Ductwork {
  class Server {
  public:
    Server(std::string path, void *userData);
    ~Server();

    bool createPipe(int defaultTimeoutMs);
    void close_pipe();
    
    std::string get_full_path();
    void set_path(std::string path);
    int get_fd();

    void *get_user_data();
    void set_user_data(void *userData);

    enum dw_instance_type get_type();
    std::string get_last_error();

  private:
    dw_instance *dw;
  };
}
