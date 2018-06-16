class IRDA {
private:
  static const unsigned long READER_TIME = 13500;
  static const unsigned long ZERO_TIME = 1125;
  static const unsigned long ONE_TIME = 2250;
  static const unsigned long REPEAT_TIME = 108000;
  static const unsigned long STOP_TIME = 11250;

  enum State {
    INIT,
    CODE,
    REPEAT,
    STOP,
  };

  State state;
  unsigned long start_time;
  unsigned long prev_time;
  unsigned long data;
  int cnt;
  void (*on_commit)(unsigned long);
  void (*on_repeat)(unsigned long);
  void (*on_error)(char*);

  bool compare(
    unsigned long diff, unsigned long target);

public:
  IRDA(
    unsigned long time,
    void (*on_commit)(unsigned long),
    void (*on_repeat)(unsigned long),
    void (*on_error)(char*));
  ~IRDA();
  void detect(unsigned long now);
};
