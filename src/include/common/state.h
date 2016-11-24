#ifndef STATE_H__
#define STATE_H__

enum State {
  kStateOK,
  kStateEOF,
  kStateParseError,
  kStateBuildError,
  kStateExecError,
  kStateFrontendTerminate,
  kStateTableNotFind,
  kStateFileNotFind,
  kStateNameAmbiguous,
  kStateAttributeNotFound,
  kStateNotSupport
};


#endif // STATE_H__
