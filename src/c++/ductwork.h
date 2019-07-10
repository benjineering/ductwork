#include <string>
#include "../ductwork.h"

namespace Ductwork {
  class Server {
  public:
    Server(std::string path, void *userData);
    ~Server();

    bool createPipe(int defaultTimeoutMs = 300);
    void closePipe();
    
    std::string getFullPath();
    void setPath(std::string path);
    int getFd();

    void *getUserData();
    void setUserData(void *userData);

    enum dw_instance_type getType();
    std::string getLastError();

  private:
    dw_instance *dw;
  };
}
