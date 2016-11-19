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
  kStateNameAmbiguous,
  kStateAttributeNotFound,
};


#endif // STATE_H__
