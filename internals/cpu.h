#ifndef CPU_H
#define CPU_H

class Cpu {
  public:
    Cpu() = default;

    bool GetStopped();
    void SetStopped(bool flag);
  private:
    bool stopped = false;
};

#endif