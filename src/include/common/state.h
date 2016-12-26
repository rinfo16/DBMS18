#ifndef STATE_H__
#define STATE_H__

enum State {
  kStateOK,
  kStateEOF,
  kStateOpenFileError,
  kStateLoadError,
  kStateParseError,
  kStateBuildError,
  kStateExecError,
  kStateFrontendTerminate,
  kStateRelationNotFound,
  kStateFileNotFind,
  kStateNameAmbiguous,
  kStateAttributeNotFound,
  kStateNotSupport,
  kStateInsertFailed,
};


#endif // STATE_H__
