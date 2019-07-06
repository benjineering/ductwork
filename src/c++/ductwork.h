#include <string>
#include "../ductwork.h"

namespace Ductwork {
  class Server {
  public:
    Server(std::string path, void *userData);
    ~Server();

    std::string get_full_path();

  private:
    dw_instance *dw;
  };
}
