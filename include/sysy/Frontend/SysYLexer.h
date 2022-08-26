
// Generated from lib/Frontend/SysYLexer.g4 by ANTLR 4.9.2

#pragma once

#include "antlr4-runtime.h"




class  SysYLexer : public antlr4::Lexer {
public:
  enum {
    INT = 1, FLOAT = 2, CONST = 3, VOID = 4, STRING = 5, ESC = 6, IF = 7, 
    ELSE = 8, WHILE = 9, FOR = 10, BREAK = 11, CONTINUE = 12, RETURN = 13, 
    AGN = 14, ADD = 15, SUB = 16, MUL = 17, DIV = 18, MOD = 19, EQL = 20, 
    NEQ = 21, LES = 22, GRT = 23, LEQ = 24, GEQ = 25, NOT = 26, AND = 27, 
    OR = 28, Ident = 29, DEC = 30, OCT = 31, HEX = 32, DECFLOAT = 33, HEXFLOAT = 34, 
    COMMA = 35, SEMI = 36, LBRA = 37, RBRA = 38, LSBRA = 39, RSBRA = 40, 
    LCBRA = 41, RCBRA = 42, WS = 43, SL_COMMENT = 44, COMMENT = 45
  };

  explicit SysYLexer(antlr4::CharStream *input);
  ~SysYLexer();

  virtual std::string getGrammarFileName() const override;
  virtual const std::vector<std::string>& getRuleNames() const override;

  virtual const std::vector<std::string>& getChannelNames() const override;
  virtual const std::vector<std::string>& getModeNames() const override;
  virtual const std::vector<std::string>& getTokenNames() const override; // deprecated, use vocabulary instead
  virtual antlr4::dfa::Vocabulary& getVocabulary() const override;

  virtual const std::vector<uint16_t> getSerializedATN() const override;
  virtual const antlr4::atn::ATN& getATN() const override;

private:
  static std::vector<antlr4::dfa::DFA> _decisionToDFA;
  static antlr4::atn::PredictionContextCache _sharedContextCache;
  static std::vector<std::string> _ruleNames;
  static std::vector<std::string> _tokenNames;
  static std::vector<std::string> _channelNames;
  static std::vector<std::string> _modeNames;

  static std::vector<std::string> _literalNames;
  static std::vector<std::string> _symbolicNames;
  static antlr4::dfa::Vocabulary _vocabulary;
  static antlr4::atn::ATN _atn;
  static std::vector<uint16_t> _serializedATN;


  // Individual action functions triggered by action() above.

  // Individual semantic predicate functions triggered by sempred() above.

  struct Initializer {
    Initializer();
  };
  static Initializer _init;
};

