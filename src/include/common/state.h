#ifndef STATE_H__
#define STATE_H__

enum State {
  kStateOK,
  kStateParseError,
  kStateBuildError,
  kStateExecError,
  kStateFrontendTerminate,
};


#endif // STATE_H__
