
// Generated from lib/Frontend/SysYParser.g4 by ANTLR 4.9.2


#include "sysy/Frontend/SysYParserVisitor.h"

#include "sysy/Frontend/SysYParser.h"


using namespace antlrcpp;
using namespace antlr4;

SysYParser::SysYParser(TokenStream *input) : Parser(input) {
  _interpreter = new atn::ParserATNSimulator(this, _atn, _decisionToDFA, _sharedContextCache);
}

SysYParser::~SysYParser() {
  delete _interpreter;
}

std::string SysYParser::getGrammarFileName() const {
  return "SysYParser.g4";
}

const std::vector<std::string>& SysYParser::getRuleNames() const {
  return _ruleNames;
}

dfa::Vocabulary& SysYParser::getVocabulary() const {
  return _vocabulary;
}


//----------------- CompUnitContext ------------------------------------------------------------------

SysYParser::CompUnitContext::CompUnitContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::CompUnitContext::EOF() {
  return getToken(SysYParser::EOF, 0);
}

std::vector<SysYParser::ElementContext *> SysYParser::CompUnitContext::element() {
  return getRuleContexts<SysYParser::ElementContext>();
}

SysYParser::ElementContext* SysYParser::CompUnitContext::element(size_t i) {
  return getRuleContext<SysYParser::ElementContext>(i);
}


size_t SysYParser::CompUnitContext::getRuleIndex() const {
  return SysYParser::RuleCompUnit;
}


antlrcpp::Any SysYParser::CompUnitContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitCompUnit(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CompUnitContext* SysYParser::compUnit() {
  CompUnitContext *_localctx = _tracker.createInstance<CompUnitContext>(_ctx, getState());
  enterRule(_localctx, 0, SysYParser::RuleCompUnit);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(75);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::INT)
      | (1ULL << SysYParser::FLOAT)
      | (1ULL << SysYParser::CONST)
      | (1ULL << SysYParser::VOID))) != 0)) {
      setState(72);
      element();
      setState(77);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(78);
    match(SysYParser::EOF);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ElementContext ------------------------------------------------------------------

SysYParser::ElementContext::ElementContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::DeclContext* SysYParser::ElementContext::decl() {
  return getRuleContext<SysYParser::DeclContext>(0);
}

SysYParser::FuncDefContext* SysYParser::ElementContext::funcDef() {
  return getRuleContext<SysYParser::FuncDefContext>(0);
}


size_t SysYParser::ElementContext::getRuleIndex() const {
  return SysYParser::RuleElement;
}


antlrcpp::Any SysYParser::ElementContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitElement(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ElementContext* SysYParser::element() {
  ElementContext *_localctx = _tracker.createInstance<ElementContext>(_ctx, getState());
  enterRule(_localctx, 2, SysYParser::RuleElement);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(82);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 1, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(80);
      decl();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(81);
      funcDef();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- DeclContext ------------------------------------------------------------------

SysYParser::DeclContext::DeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ConstDeclContext* SysYParser::DeclContext::constDecl() {
  return getRuleContext<SysYParser::ConstDeclContext>(0);
}

SysYParser::VarDeclContext* SysYParser::DeclContext::varDecl() {
  return getRuleContext<SysYParser::VarDeclContext>(0);
}


size_t SysYParser::DeclContext::getRuleIndex() const {
  return SysYParser::RuleDecl;
}


antlrcpp::Any SysYParser::DeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::DeclContext* SysYParser::decl() {
  DeclContext *_localctx = _tracker.createInstance<DeclContext>(_ctx, getState());
  enterRule(_localctx, 4, SysYParser::RuleDecl);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(86);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::CONST: {
        enterOuterAlt(_localctx, 1);
        setState(84);
        constDecl();
        break;
      }

      case SysYParser::INT:
      case SysYParser::FLOAT: {
        enterOuterAlt(_localctx, 2);
        setState(85);
        varDecl();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDeclContext ------------------------------------------------------------------

SysYParser::ConstDeclContext::ConstDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ConstDeclContext::CONST() {
  return getToken(SysYParser::CONST, 0);
}

SysYParser::BTypeContext* SysYParser::ConstDeclContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

std::vector<SysYParser::ConstDefContext *> SysYParser::ConstDeclContext::constDef() {
  return getRuleContexts<SysYParser::ConstDefContext>();
}

SysYParser::ConstDefContext* SysYParser::ConstDeclContext::constDef(size_t i) {
  return getRuleContext<SysYParser::ConstDefContext>(i);
}

tree::TerminalNode* SysYParser::ConstDeclContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDeclContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::ConstDeclContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::ConstDeclContext::getRuleIndex() const {
  return SysYParser::RuleConstDecl;
}


antlrcpp::Any SysYParser::ConstDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstDeclContext* SysYParser::constDecl() {
  ConstDeclContext *_localctx = _tracker.createInstance<ConstDeclContext>(_ctx, getState());
  enterRule(_localctx, 6, SysYParser::RuleConstDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(88);
    match(SysYParser::CONST);
    setState(89);
    bType();
    setState(90);
    constDef();
    setState(95);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(91);
      match(SysYParser::COMMA);
      setState(92);
      constDef();
      setState(97);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(98);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BTypeContext ------------------------------------------------------------------

SysYParser::BTypeContext::BTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BTypeContext::INT() {
  return getToken(SysYParser::INT, 0);
}

tree::TerminalNode* SysYParser::BTypeContext::FLOAT() {
  return getToken(SysYParser::FLOAT, 0);
}


size_t SysYParser::BTypeContext::getRuleIndex() const {
  return SysYParser::RuleBType;
}


antlrcpp::Any SysYParser::BTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BTypeContext* SysYParser::bType() {
  BTypeContext *_localctx = _tracker.createInstance<BTypeContext>(_ctx, getState());
  enterRule(_localctx, 8, SysYParser::RuleBType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(100);
    _la = _input->LA(1);
    if (!(_la == SysYParser::INT

    || _la == SysYParser::FLOAT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstDefContext ------------------------------------------------------------------

SysYParser::ConstDefContext::ConstDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::ConstDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::ConstDefContext::AGN() {
  return getToken(SysYParser::AGN, 0);
}

SysYParser::ConstInitValContext* SysYParser::ConstDefContext::constInitVal() {
  return getRuleContext<SysYParser::ConstInitValContext>(0);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDefContext::LSBRA() {
  return getTokens(SysYParser::LSBRA);
}

tree::TerminalNode* SysYParser::ConstDefContext::LSBRA(size_t i) {
  return getToken(SysYParser::LSBRA, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::ConstDefContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::ConstDefContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ConstDefContext::RSBRA() {
  return getTokens(SysYParser::RSBRA);
}

tree::TerminalNode* SysYParser::ConstDefContext::RSBRA(size_t i) {
  return getToken(SysYParser::RSBRA, i);
}


size_t SysYParser::ConstDefContext::getRuleIndex() const {
  return SysYParser::RuleConstDef;
}


antlrcpp::Any SysYParser::ConstDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstDefContext* SysYParser::constDef() {
  ConstDefContext *_localctx = _tracker.createInstance<ConstDefContext>(_ctx, getState());
  enterRule(_localctx, 10, SysYParser::RuleConstDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(102);
    match(SysYParser::Ident);
    setState(109);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::LSBRA) {
      setState(103);
      match(SysYParser::LSBRA);
      setState(104);
      constExp();
      setState(105);
      match(SysYParser::RSBRA);
      setState(111);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(112);
    match(SysYParser::AGN);
    setState(113);
    constInitVal();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ConstInitValContext ------------------------------------------------------------------

SysYParser::ConstInitValContext::ConstInitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ConstExpContext* SysYParser::ConstInitValContext::constExp() {
  return getRuleContext<SysYParser::ConstExpContext>(0);
}

tree::TerminalNode* SysYParser::ConstInitValContext::LCBRA() {
  return getToken(SysYParser::LCBRA, 0);
}

tree::TerminalNode* SysYParser::ConstInitValContext::RCBRA() {
  return getToken(SysYParser::RCBRA, 0);
}

std::vector<SysYParser::ConstInitValContext *> SysYParser::ConstInitValContext::constInitVal() {
  return getRuleContexts<SysYParser::ConstInitValContext>();
}

SysYParser::ConstInitValContext* SysYParser::ConstInitValContext::constInitVal(size_t i) {
  return getRuleContext<SysYParser::ConstInitValContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::ConstInitValContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::ConstInitValContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::ConstInitValContext::getRuleIndex() const {
  return SysYParser::RuleConstInitVal;
}


antlrcpp::Any SysYParser::ConstInitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstInitVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstInitValContext* SysYParser::constInitVal() {
  ConstInitValContext *_localctx = _tracker.createInstance<ConstInitValContext>(_ctx, getState());
  enterRule(_localctx, 12, SysYParser::RuleConstInitVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(128);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::Ident:
      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX:
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT:
      case SysYParser::LBRA: {
        enterOuterAlt(_localctx, 1);
        setState(115);
        constExp();
        break;
      }

      case SysYParser::LCBRA: {
        enterOuterAlt(_localctx, 2);
        setState(116);
        match(SysYParser::LCBRA);
        setState(125);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::ADD)
          | (1ULL << SysYParser::SUB)
          | (1ULL << SysYParser::NOT)
          | (1ULL << SysYParser::Ident)
          | (1ULL << SysYParser::DEC)
          | (1ULL << SysYParser::OCT)
          | (1ULL << SysYParser::HEX)
          | (1ULL << SysYParser::DECFLOAT)
          | (1ULL << SysYParser::HEXFLOAT)
          | (1ULL << SysYParser::LBRA)
          | (1ULL << SysYParser::LCBRA))) != 0)) {
          setState(117);
          constInitVal();
          setState(122);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParser::COMMA) {
            setState(118);
            match(SysYParser::COMMA);
            setState(119);
            constInitVal();
            setState(124);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(127);
        match(SysYParser::RCBRA);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDeclContext ------------------------------------------------------------------

SysYParser::VarDeclContext::VarDeclContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BTypeContext* SysYParser::VarDeclContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

std::vector<SysYParser::VarDefContext *> SysYParser::VarDeclContext::varDef() {
  return getRuleContexts<SysYParser::VarDefContext>();
}

SysYParser::VarDefContext* SysYParser::VarDeclContext::varDef(size_t i) {
  return getRuleContext<SysYParser::VarDefContext>(i);
}

tree::TerminalNode* SysYParser::VarDeclContext::SEMI() {
  return getToken(SysYParser::SEMI, 0);
}

std::vector<tree::TerminalNode *> SysYParser::VarDeclContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::VarDeclContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::VarDeclContext::getRuleIndex() const {
  return SysYParser::RuleVarDecl;
}


antlrcpp::Any SysYParser::VarDeclContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDecl(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDeclContext* SysYParser::varDecl() {
  VarDeclContext *_localctx = _tracker.createInstance<VarDeclContext>(_ctx, getState());
  enterRule(_localctx, 14, SysYParser::RuleVarDecl);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(130);
    bType();
    setState(131);
    varDef();
    setState(136);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(132);
      match(SysYParser::COMMA);
      setState(133);
      varDef();
      setState(138);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(139);
    match(SysYParser::SEMI);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDecl_inContext ------------------------------------------------------------------

SysYParser::VarDecl_inContext::VarDecl_inContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BTypeContext* SysYParser::VarDecl_inContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

std::vector<SysYParser::VarDefContext *> SysYParser::VarDecl_inContext::varDef() {
  return getRuleContexts<SysYParser::VarDefContext>();
}

SysYParser::VarDefContext* SysYParser::VarDecl_inContext::varDef(size_t i) {
  return getRuleContext<SysYParser::VarDefContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::VarDecl_inContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::VarDecl_inContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::VarDecl_inContext::getRuleIndex() const {
  return SysYParser::RuleVarDecl_in;
}


antlrcpp::Any SysYParser::VarDecl_inContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDecl_in(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDecl_inContext* SysYParser::varDecl_in() {
  VarDecl_inContext *_localctx = _tracker.createInstance<VarDecl_inContext>(_ctx, getState());
  enterRule(_localctx, 16, SysYParser::RuleVarDecl_in);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(141);
    bType();
    setState(142);
    varDef();
    setState(147);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(143);
      match(SysYParser::COMMA);
      setState(144);
      varDef();
      setState(149);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- VarDefContext ------------------------------------------------------------------

SysYParser::VarDefContext::VarDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::VarDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::VarDefContext::LSBRA() {
  return getTokens(SysYParser::LSBRA);
}

tree::TerminalNode* SysYParser::VarDefContext::LSBRA(size_t i) {
  return getToken(SysYParser::LSBRA, i);
}

std::vector<SysYParser::ConstExpContext *> SysYParser::VarDefContext::constExp() {
  return getRuleContexts<SysYParser::ConstExpContext>();
}

SysYParser::ConstExpContext* SysYParser::VarDefContext::constExp(size_t i) {
  return getRuleContext<SysYParser::ConstExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::VarDefContext::RSBRA() {
  return getTokens(SysYParser::RSBRA);
}

tree::TerminalNode* SysYParser::VarDefContext::RSBRA(size_t i) {
  return getToken(SysYParser::RSBRA, i);
}

tree::TerminalNode* SysYParser::VarDefContext::AGN() {
  return getToken(SysYParser::AGN, 0);
}

SysYParser::InitValContext* SysYParser::VarDefContext::initVal() {
  return getRuleContext<SysYParser::InitValContext>(0);
}


size_t SysYParser::VarDefContext::getRuleIndex() const {
  return SysYParser::RuleVarDef;
}


antlrcpp::Any SysYParser::VarDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitVarDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::VarDefContext* SysYParser::varDef() {
  VarDefContext *_localctx = _tracker.createInstance<VarDefContext>(_ctx, getState());
  enterRule(_localctx, 18, SysYParser::RuleVarDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(172);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 12, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(150);
      match(SysYParser::Ident);
      setState(157);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::LSBRA) {
        setState(151);
        match(SysYParser::LSBRA);
        setState(152);
        constExp();
        setState(153);
        match(SysYParser::RSBRA);
        setState(159);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(160);
      match(SysYParser::Ident);
      setState(167);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::LSBRA) {
        setState(161);
        match(SysYParser::LSBRA);
        setState(162);
        constExp();
        setState(163);
        match(SysYParser::RSBRA);
        setState(169);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
      setState(170);
      match(SysYParser::AGN);
      setState(171);
      initVal();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- InitValContext ------------------------------------------------------------------

SysYParser::InitValContext::InitValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::ExpContext* SysYParser::InitValContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::InitValContext::LCBRA() {
  return getToken(SysYParser::LCBRA, 0);
}

tree::TerminalNode* SysYParser::InitValContext::RCBRA() {
  return getToken(SysYParser::RCBRA, 0);
}

std::vector<SysYParser::InitValContext *> SysYParser::InitValContext::initVal() {
  return getRuleContexts<SysYParser::InitValContext>();
}

SysYParser::InitValContext* SysYParser::InitValContext::initVal(size_t i) {
  return getRuleContext<SysYParser::InitValContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::InitValContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::InitValContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::InitValContext::getRuleIndex() const {
  return SysYParser::RuleInitVal;
}


antlrcpp::Any SysYParser::InitValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitInitVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::InitValContext* SysYParser::initVal() {
  InitValContext *_localctx = _tracker.createInstance<InitValContext>(_ctx, getState());
  enterRule(_localctx, 20, SysYParser::RuleInitVal);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(187);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::Ident:
      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX:
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT:
      case SysYParser::LBRA: {
        enterOuterAlt(_localctx, 1);
        setState(174);
        exp();
        break;
      }

      case SysYParser::LCBRA: {
        enterOuterAlt(_localctx, 2);
        setState(175);
        match(SysYParser::LCBRA);
        setState(184);
        _errHandler->sync(this);

        _la = _input->LA(1);
        if ((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::ADD)
          | (1ULL << SysYParser::SUB)
          | (1ULL << SysYParser::NOT)
          | (1ULL << SysYParser::Ident)
          | (1ULL << SysYParser::DEC)
          | (1ULL << SysYParser::OCT)
          | (1ULL << SysYParser::HEX)
          | (1ULL << SysYParser::DECFLOAT)
          | (1ULL << SysYParser::HEXFLOAT)
          | (1ULL << SysYParser::LBRA)
          | (1ULL << SysYParser::LCBRA))) != 0)) {
          setState(176);
          initVal();
          setState(181);
          _errHandler->sync(this);
          _la = _input->LA(1);
          while (_la == SysYParser::COMMA) {
            setState(177);
            match(SysYParser::COMMA);
            setState(178);
            initVal();
            setState(183);
            _errHandler->sync(this);
            _la = _input->LA(1);
          }
        }
        setState(186);
        match(SysYParser::RCBRA);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncDefContext ------------------------------------------------------------------

SysYParser::FuncDefContext::FuncDefContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::FuncTypeContext* SysYParser::FuncDefContext::funcType() {
  return getRuleContext<SysYParser::FuncTypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncDefContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::LBRA() {
  return getToken(SysYParser::LBRA, 0);
}

tree::TerminalNode* SysYParser::FuncDefContext::RBRA() {
  return getToken(SysYParser::RBRA, 0);
}

SysYParser::BlockContext* SysYParser::FuncDefContext::block() {
  return getRuleContext<SysYParser::BlockContext>(0);
}

SysYParser::FuncFParamsContext* SysYParser::FuncDefContext::funcFParams() {
  return getRuleContext<SysYParser::FuncFParamsContext>(0);
}


size_t SysYParser::FuncDefContext::getRuleIndex() const {
  return SysYParser::RuleFuncDef;
}


antlrcpp::Any SysYParser::FuncDefContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncDef(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncDefContext* SysYParser::funcDef() {
  FuncDefContext *_localctx = _tracker.createInstance<FuncDefContext>(_ctx, getState());
  enterRule(_localctx, 22, SysYParser::RuleFuncDef);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(189);
    funcType();
    setState(190);
    match(SysYParser::Ident);
    setState(191);
    match(SysYParser::LBRA);
    setState(193);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::INT

    || _la == SysYParser::FLOAT) {
      setState(192);
      funcFParams();
    }
    setState(195);
    match(SysYParser::RBRA);
    setState(196);
    block();
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncTypeContext ------------------------------------------------------------------

SysYParser::FuncTypeContext::FuncTypeContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::FuncTypeContext::VOID() {
  return getToken(SysYParser::VOID, 0);
}

tree::TerminalNode* SysYParser::FuncTypeContext::INT() {
  return getToken(SysYParser::INT, 0);
}

tree::TerminalNode* SysYParser::FuncTypeContext::FLOAT() {
  return getToken(SysYParser::FLOAT, 0);
}


size_t SysYParser::FuncTypeContext::getRuleIndex() const {
  return SysYParser::RuleFuncType;
}


antlrcpp::Any SysYParser::FuncTypeContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncType(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncTypeContext* SysYParser::funcType() {
  FuncTypeContext *_localctx = _tracker.createInstance<FuncTypeContext>(_ctx, getState());
  enterRule(_localctx, 24, SysYParser::RuleFuncType);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(198);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::INT)
      | (1ULL << SysYParser::FLOAT)
      | (1ULL << SysYParser::VOID))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamsContext ------------------------------------------------------------------

SysYParser::FuncFParamsContext::FuncFParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::FuncFParamContext *> SysYParser::FuncFParamsContext::funcFParam() {
  return getRuleContexts<SysYParser::FuncFParamContext>();
}

SysYParser::FuncFParamContext* SysYParser::FuncFParamsContext::funcFParam(size_t i) {
  return getRuleContext<SysYParser::FuncFParamContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamsContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncFParamsContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncFParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParams;
}


antlrcpp::Any SysYParser::FuncFParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncFParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamsContext* SysYParser::funcFParams() {
  FuncFParamsContext *_localctx = _tracker.createInstance<FuncFParamsContext>(_ctx, getState());
  enterRule(_localctx, 26, SysYParser::RuleFuncFParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(200);
    funcFParam();
    setState(205);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(201);
      match(SysYParser::COMMA);
      setState(202);
      funcFParam();
      setState(207);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncFParamContext ------------------------------------------------------------------

SysYParser::FuncFParamContext::FuncFParamContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::BTypeContext* SysYParser::FuncFParamContext::bType() {
  return getRuleContext<SysYParser::BTypeContext>(0);
}

tree::TerminalNode* SysYParser::FuncFParamContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::LSBRA() {
  return getTokens(SysYParser::LSBRA);
}

tree::TerminalNode* SysYParser::FuncFParamContext::LSBRA(size_t i) {
  return getToken(SysYParser::LSBRA, i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncFParamContext::RSBRA() {
  return getTokens(SysYParser::RSBRA);
}

tree::TerminalNode* SysYParser::FuncFParamContext::RSBRA(size_t i) {
  return getToken(SysYParser::RSBRA, i);
}

std::vector<SysYParser::ExpContext *> SysYParser::FuncFParamContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::FuncFParamContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}


size_t SysYParser::FuncFParamContext::getRuleIndex() const {
  return SysYParser::RuleFuncFParam;
}


antlrcpp::Any SysYParser::FuncFParamContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncFParam(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncFParamContext* SysYParser::funcFParam() {
  FuncFParamContext *_localctx = _tracker.createInstance<FuncFParamContext>(_ctx, getState());
  enterRule(_localctx, 28, SysYParser::RuleFuncFParam);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(208);
    bType();
    setState(209);
    match(SysYParser::Ident);
    setState(221);
    _errHandler->sync(this);

    _la = _input->LA(1);
    if (_la == SysYParser::LSBRA) {
      setState(210);
      match(SysYParser::LSBRA);
      setState(211);
      match(SysYParser::RSBRA);
      setState(218);
      _errHandler->sync(this);
      _la = _input->LA(1);
      while (_la == SysYParser::LSBRA) {
        setState(212);
        match(SysYParser::LSBRA);
        setState(213);
        exp();
        setState(214);
        match(SysYParser::RSBRA);
        setState(220);
        _errHandler->sync(this);
        _la = _input->LA(1);
      }
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockContext ------------------------------------------------------------------

SysYParser::BlockContext::BlockContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::BlockContext::LCBRA() {
  return getToken(SysYParser::LCBRA, 0);
}

tree::TerminalNode* SysYParser::BlockContext::RCBRA() {
  return getToken(SysYParser::RCBRA, 0);
}

std::vector<SysYParser::BlockItemContext *> SysYParser::BlockContext::blockItem() {
  return getRuleContexts<SysYParser::BlockItemContext>();
}

SysYParser::BlockItemContext* SysYParser::BlockContext::blockItem(size_t i) {
  return getRuleContext<SysYParser::BlockItemContext>(i);
}


size_t SysYParser::BlockContext::getRuleIndex() const {
  return SysYParser::RuleBlock;
}


antlrcpp::Any SysYParser::BlockContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBlock(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockContext* SysYParser::block() {
  BlockContext *_localctx = _tracker.createInstance<BlockContext>(_ctx, getState());
  enterRule(_localctx, 30, SysYParser::RuleBlock);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(223);
    match(SysYParser::LCBRA);
    setState(227);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while ((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::INT)
      | (1ULL << SysYParser::FLOAT)
      | (1ULL << SysYParser::CONST)
      | (1ULL << SysYParser::IF)
      | (1ULL << SysYParser::WHILE)
      | (1ULL << SysYParser::FOR)
      | (1ULL << SysYParser::BREAK)
      | (1ULL << SysYParser::CONTINUE)
      | (1ULL << SysYParser::RETURN)
      | (1ULL << SysYParser::ADD)
      | (1ULL << SysYParser::SUB)
      | (1ULL << SysYParser::NOT)
      | (1ULL << SysYParser::Ident)
      | (1ULL << SysYParser::DEC)
      | (1ULL << SysYParser::OCT)
      | (1ULL << SysYParser::HEX)
      | (1ULL << SysYParser::DECFLOAT)
      | (1ULL << SysYParser::HEXFLOAT)
      | (1ULL << SysYParser::SEMI)
      | (1ULL << SysYParser::LBRA)
      | (1ULL << SysYParser::LCBRA))) != 0)) {
      setState(224);
      blockItem();
      setState(229);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
    setState(230);
    match(SysYParser::RCBRA);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- BlockItemContext ------------------------------------------------------------------

SysYParser::BlockItemContext::BlockItemContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::DeclContext* SysYParser::BlockItemContext::decl() {
  return getRuleContext<SysYParser::DeclContext>(0);
}

SysYParser::StmtContext* SysYParser::BlockItemContext::stmt() {
  return getRuleContext<SysYParser::StmtContext>(0);
}


size_t SysYParser::BlockItemContext::getRuleIndex() const {
  return SysYParser::RuleBlockItem;
}


antlrcpp::Any SysYParser::BlockItemContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitBlockItem(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::BlockItemContext* SysYParser::blockItem() {
  BlockItemContext *_localctx = _tracker.createInstance<BlockItemContext>(_ctx, getState());
  enterRule(_localctx, 32, SysYParser::RuleBlockItem);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(234);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::INT:
      case SysYParser::FLOAT:
      case SysYParser::CONST: {
        enterOuterAlt(_localctx, 1);
        setState(232);
        decl();
        break;
      }

      case SysYParser::IF:
      case SysYParser::WHILE:
      case SysYParser::FOR:
      case SysYParser::BREAK:
      case SysYParser::CONTINUE:
      case SysYParser::RETURN:
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::Ident:
      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX:
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT:
      case SysYParser::SEMI:
      case SysYParser::LBRA:
      case SysYParser::LCBRA: {
        enterOuterAlt(_localctx, 2);
        setState(233);
        stmt();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- Stmt_inContext ------------------------------------------------------------------

SysYParser::Stmt_inContext::Stmt_inContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValContext* SysYParser::Stmt_inContext::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

tree::TerminalNode* SysYParser::Stmt_inContext::AGN() {
  return getToken(SysYParser::AGN, 0);
}

SysYParser::ExpContext* SysYParser::Stmt_inContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}


size_t SysYParser::Stmt_inContext::getRuleIndex() const {
  return SysYParser::RuleStmt_in;
}


antlrcpp::Any SysYParser::Stmt_inContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmt_in(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::Stmt_inContext* SysYParser::stmt_in() {
  Stmt_inContext *_localctx = _tracker.createInstance<Stmt_inContext>(_ctx, getState());
  enterRule(_localctx, 34, SysYParser::RuleStmt_in);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(243);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 23, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(236);
      lVal();
      setState(237);
      match(SysYParser::AGN);
      setState(238);
      exp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(241);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParser::ADD)
        | (1ULL << SysYParser::SUB)
        | (1ULL << SysYParser::NOT)
        | (1ULL << SysYParser::Ident)
        | (1ULL << SysYParser::DEC)
        | (1ULL << SysYParser::OCT)
        | (1ULL << SysYParser::HEX)
        | (1ULL << SysYParser::DECFLOAT)
        | (1ULL << SysYParser::HEXFLOAT)
        | (1ULL << SysYParser::LBRA))) != 0)) {
        setState(240);
        exp();
      }
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- StmtContext ------------------------------------------------------------------

SysYParser::StmtContext::StmtContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LValContext* SysYParser::StmtContext::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

tree::TerminalNode* SysYParser::StmtContext::AGN() {
  return getToken(SysYParser::AGN, 0);
}

SysYParser::ExpContext* SysYParser::StmtContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

std::vector<tree::TerminalNode *> SysYParser::StmtContext::SEMI() {
  return getTokens(SysYParser::SEMI);
}

tree::TerminalNode* SysYParser::StmtContext::SEMI(size_t i) {
  return getToken(SysYParser::SEMI, i);
}

SysYParser::BlockContext* SysYParser::StmtContext::block() {
  return getRuleContext<SysYParser::BlockContext>(0);
}

tree::TerminalNode* SysYParser::StmtContext::IF() {
  return getToken(SysYParser::IF, 0);
}

tree::TerminalNode* SysYParser::StmtContext::LBRA() {
  return getToken(SysYParser::LBRA, 0);
}

SysYParser::CondContext* SysYParser::StmtContext::cond() {
  return getRuleContext<SysYParser::CondContext>(0);
}

tree::TerminalNode* SysYParser::StmtContext::RBRA() {
  return getToken(SysYParser::RBRA, 0);
}

std::vector<SysYParser::StmtContext *> SysYParser::StmtContext::stmt() {
  return getRuleContexts<SysYParser::StmtContext>();
}

SysYParser::StmtContext* SysYParser::StmtContext::stmt(size_t i) {
  return getRuleContext<SysYParser::StmtContext>(i);
}

tree::TerminalNode* SysYParser::StmtContext::ELSE() {
  return getToken(SysYParser::ELSE, 0);
}

tree::TerminalNode* SysYParser::StmtContext::FOR() {
  return getToken(SysYParser::FOR, 0);
}

std::vector<SysYParser::Stmt_inContext *> SysYParser::StmtContext::stmt_in() {
  return getRuleContexts<SysYParser::Stmt_inContext>();
}

SysYParser::Stmt_inContext* SysYParser::StmtContext::stmt_in(size_t i) {
  return getRuleContext<SysYParser::Stmt_inContext>(i);
}

SysYParser::VarDecl_inContext* SysYParser::StmtContext::varDecl_in() {
  return getRuleContext<SysYParser::VarDecl_inContext>(0);
}

tree::TerminalNode* SysYParser::StmtContext::WHILE() {
  return getToken(SysYParser::WHILE, 0);
}

tree::TerminalNode* SysYParser::StmtContext::BREAK() {
  return getToken(SysYParser::BREAK, 0);
}

tree::TerminalNode* SysYParser::StmtContext::CONTINUE() {
  return getToken(SysYParser::CONTINUE, 0);
}

tree::TerminalNode* SysYParser::StmtContext::RETURN() {
  return getToken(SysYParser::RETURN, 0);
}


size_t SysYParser::StmtContext::getRuleIndex() const {
  return SysYParser::RuleStmt;
}


antlrcpp::Any SysYParser::StmtContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitStmt(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::StmtContext* SysYParser::stmt() {
  StmtContext *_localctx = _tracker.createInstance<StmtContext>(_ctx, getState());
  enterRule(_localctx, 36, SysYParser::RuleStmt);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(292);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 28, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(245);
      lVal();
      setState(246);
      match(SysYParser::AGN);
      setState(247);
      exp();
      setState(248);
      match(SysYParser::SEMI);
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(251);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParser::ADD)
        | (1ULL << SysYParser::SUB)
        | (1ULL << SysYParser::NOT)
        | (1ULL << SysYParser::Ident)
        | (1ULL << SysYParser::DEC)
        | (1ULL << SysYParser::OCT)
        | (1ULL << SysYParser::HEX)
        | (1ULL << SysYParser::DECFLOAT)
        | (1ULL << SysYParser::HEXFLOAT)
        | (1ULL << SysYParser::LBRA))) != 0)) {
        setState(250);
        exp();
      }
      setState(253);
      match(SysYParser::SEMI);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(254);
      block();
      break;
    }

    case 4: {
      enterOuterAlt(_localctx, 4);
      setState(255);
      match(SysYParser::IF);
      setState(256);
      match(SysYParser::LBRA);
      setState(257);
      cond();
      setState(258);
      match(SysYParser::RBRA);
      setState(259);
      stmt();
      setState(262);
      _errHandler->sync(this);

      switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 25, _ctx)) {
      case 1: {
        setState(260);
        match(SysYParser::ELSE);
        setState(261);
        stmt();
        break;
      }

      default:
        break;
      }
      break;
    }

    case 5: {
      enterOuterAlt(_localctx, 5);
      setState(264);
      match(SysYParser::FOR);
      setState(265);
      match(SysYParser::LBRA);
      setState(268);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case SysYParser::ADD:
        case SysYParser::SUB:
        case SysYParser::NOT:
        case SysYParser::Ident:
        case SysYParser::DEC:
        case SysYParser::OCT:
        case SysYParser::HEX:
        case SysYParser::DECFLOAT:
        case SysYParser::HEXFLOAT:
        case SysYParser::SEMI:
        case SysYParser::LBRA: {
          setState(266);
          stmt_in();
          break;
        }

        case SysYParser::INT:
        case SysYParser::FLOAT: {
          setState(267);
          varDecl_in();
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(270);
      match(SysYParser::SEMI);
      setState(271);
      cond();
      setState(272);
      match(SysYParser::SEMI);
      setState(273);
      stmt_in();
      setState(274);
      match(SysYParser::RBRA);
      setState(275);
      stmt();
      break;
    }

    case 6: {
      enterOuterAlt(_localctx, 6);
      setState(277);
      match(SysYParser::WHILE);
      setState(278);
      match(SysYParser::LBRA);
      setState(279);
      cond();
      setState(280);
      match(SysYParser::RBRA);
      setState(281);
      stmt();
      break;
    }

    case 7: {
      enterOuterAlt(_localctx, 7);
      setState(283);
      match(SysYParser::BREAK);
      setState(284);
      match(SysYParser::SEMI);
      break;
    }

    case 8: {
      enterOuterAlt(_localctx, 8);
      setState(285);
      match(SysYParser::CONTINUE);
      setState(286);
      match(SysYParser::SEMI);
      break;
    }

    case 9: {
      enterOuterAlt(_localctx, 9);
      setState(287);
      match(SysYParser::RETURN);
      setState(289);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParser::ADD)
        | (1ULL << SysYParser::SUB)
        | (1ULL << SysYParser::NOT)
        | (1ULL << SysYParser::Ident)
        | (1ULL << SysYParser::DEC)
        | (1ULL << SysYParser::OCT)
        | (1ULL << SysYParser::HEX)
        | (1ULL << SysYParser::DECFLOAT)
        | (1ULL << SysYParser::HEXFLOAT)
        | (1ULL << SysYParser::LBRA))) != 0)) {
        setState(288);
        exp();
      }
      setState(291);
      match(SysYParser::SEMI);
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- ExpContext ------------------------------------------------------------------

SysYParser::ExpContext::ExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AddExpContext* SysYParser::ExpContext::addExp() {
  return getRuleContext<SysYParser::AddExpContext>(0);
}


size_t SysYParser::ExpContext::getRuleIndex() const {
  return SysYParser::RuleExp;
}


antlrcpp::Any SysYParser::ExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ExpContext* SysYParser::exp() {
  ExpContext *_localctx = _tracker.createInstance<ExpContext>(_ctx, getState());
  enterRule(_localctx, 38, SysYParser::RuleExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(294);
    addExp(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- CondContext ------------------------------------------------------------------

SysYParser::CondContext::CondContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LOrExpContext* SysYParser::CondContext::lOrExp() {
  return getRuleContext<SysYParser::LOrExpContext>(0);
}


size_t SysYParser::CondContext::getRuleIndex() const {
  return SysYParser::RuleCond;
}


antlrcpp::Any SysYParser::CondContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitCond(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::CondContext* SysYParser::cond() {
  CondContext *_localctx = _tracker.createInstance<CondContext>(_ctx, getState());
  enterRule(_localctx, 40, SysYParser::RuleCond);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(296);
    lOrExp(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- LValContext ------------------------------------------------------------------

SysYParser::LValContext::LValContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::LValContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::LSBRA() {
  return getTokens(SysYParser::LSBRA);
}

tree::TerminalNode* SysYParser::LValContext::LSBRA(size_t i) {
  return getToken(SysYParser::LSBRA, i);
}

std::vector<SysYParser::ExpContext *> SysYParser::LValContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::LValContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::LValContext::RSBRA() {
  return getTokens(SysYParser::RSBRA);
}

tree::TerminalNode* SysYParser::LValContext::RSBRA(size_t i) {
  return getToken(SysYParser::RSBRA, i);
}


size_t SysYParser::LValContext::getRuleIndex() const {
  return SysYParser::RuleLVal;
}


antlrcpp::Any SysYParser::LValContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLVal(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::LValContext* SysYParser::lVal() {
  LValContext *_localctx = _tracker.createInstance<LValContext>(_ctx, getState());
  enterRule(_localctx, 42, SysYParser::RuleLVal);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(298);
    match(SysYParser::Ident);
    setState(305);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        setState(299);
        match(SysYParser::LSBRA);
        setState(300);
        exp();
        setState(301);
        match(SysYParser::RSBRA); 
      }
      setState(307);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 29, _ctx);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- PrimaryExpContext ------------------------------------------------------------------

SysYParser::PrimaryExpContext::PrimaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::PrimaryExpContext::LBRA() {
  return getToken(SysYParser::LBRA, 0);
}

SysYParser::ExpContext* SysYParser::PrimaryExpContext::exp() {
  return getRuleContext<SysYParser::ExpContext>(0);
}

tree::TerminalNode* SysYParser::PrimaryExpContext::RBRA() {
  return getToken(SysYParser::RBRA, 0);
}

SysYParser::LValContext* SysYParser::PrimaryExpContext::lVal() {
  return getRuleContext<SysYParser::LValContext>(0);
}

SysYParser::NumberContext* SysYParser::PrimaryExpContext::number() {
  return getRuleContext<SysYParser::NumberContext>(0);
}


size_t SysYParser::PrimaryExpContext::getRuleIndex() const {
  return SysYParser::RulePrimaryExp;
}


antlrcpp::Any SysYParser::PrimaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitPrimaryExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::PrimaryExpContext* SysYParser::primaryExp() {
  PrimaryExpContext *_localctx = _tracker.createInstance<PrimaryExpContext>(_ctx, getState());
  enterRule(_localctx, 44, SysYParser::RulePrimaryExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(314);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::LBRA: {
        enterOuterAlt(_localctx, 1);
        setState(308);
        match(SysYParser::LBRA);
        setState(309);
        exp();
        setState(310);
        match(SysYParser::RBRA);
        break;
      }

      case SysYParser::Ident: {
        enterOuterAlt(_localctx, 2);
        setState(312);
        lVal();
        break;
      }

      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX:
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT: {
        enterOuterAlt(_localctx, 3);
        setState(313);
        number();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- NumberContext ------------------------------------------------------------------

SysYParser::NumberContext::NumberContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::FloatConstContext* SysYParser::NumberContext::floatConst() {
  return getRuleContext<SysYParser::FloatConstContext>(0);
}

SysYParser::IntConstContext* SysYParser::NumberContext::intConst() {
  return getRuleContext<SysYParser::IntConstContext>(0);
}


size_t SysYParser::NumberContext::getRuleIndex() const {
  return SysYParser::RuleNumber;
}


antlrcpp::Any SysYParser::NumberContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitNumber(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::NumberContext* SysYParser::number() {
  NumberContext *_localctx = _tracker.createInstance<NumberContext>(_ctx, getState());
  enterRule(_localctx, 46, SysYParser::RuleNumber);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(318);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT: {
        enterOuterAlt(_localctx, 1);
        setState(316);
        floatConst();
        break;
      }

      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX: {
        enterOuterAlt(_localctx, 2);
        setState(317);
        intConst();
        break;
      }

    default:
      throw NoViableAltException(this);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryExpContext ------------------------------------------------------------------

SysYParser::UnaryExpContext::UnaryExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::PrimaryExpContext* SysYParser::UnaryExpContext::primaryExp() {
  return getRuleContext<SysYParser::PrimaryExpContext>(0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::Ident() {
  return getToken(SysYParser::Ident, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::LBRA() {
  return getToken(SysYParser::LBRA, 0);
}

tree::TerminalNode* SysYParser::UnaryExpContext::RBRA() {
  return getToken(SysYParser::RBRA, 0);
}

SysYParser::FuncRParamsContext* SysYParser::UnaryExpContext::funcRParams() {
  return getRuleContext<SysYParser::FuncRParamsContext>(0);
}

SysYParser::UnaryOpContext* SysYParser::UnaryExpContext::unaryOp() {
  return getRuleContext<SysYParser::UnaryOpContext>(0);
}

SysYParser::UnaryExpContext* SysYParser::UnaryExpContext::unaryExp() {
  return getRuleContext<SysYParser::UnaryExpContext>(0);
}


size_t SysYParser::UnaryExpContext::getRuleIndex() const {
  return SysYParser::RuleUnaryExp;
}


antlrcpp::Any SysYParser::UnaryExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::UnaryExpContext* SysYParser::unaryExp() {
  UnaryExpContext *_localctx = _tracker.createInstance<UnaryExpContext>(_ctx, getState());
  enterRule(_localctx, 48, SysYParser::RuleUnaryExp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    setState(330);
    _errHandler->sync(this);
    switch (getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 33, _ctx)) {
    case 1: {
      enterOuterAlt(_localctx, 1);
      setState(320);
      primaryExp();
      break;
    }

    case 2: {
      enterOuterAlt(_localctx, 2);
      setState(321);
      match(SysYParser::Ident);
      setState(322);
      match(SysYParser::LBRA);
      setState(324);
      _errHandler->sync(this);

      _la = _input->LA(1);
      if ((((_la & ~ 0x3fULL) == 0) &&
        ((1ULL << _la) & ((1ULL << SysYParser::STRING)
        | (1ULL << SysYParser::ADD)
        | (1ULL << SysYParser::SUB)
        | (1ULL << SysYParser::NOT)
        | (1ULL << SysYParser::Ident)
        | (1ULL << SysYParser::DEC)
        | (1ULL << SysYParser::OCT)
        | (1ULL << SysYParser::HEX)
        | (1ULL << SysYParser::DECFLOAT)
        | (1ULL << SysYParser::HEXFLOAT)
        | (1ULL << SysYParser::LBRA))) != 0)) {
        setState(323);
        funcRParams();
      }
      setState(326);
      match(SysYParser::RBRA);
      break;
    }

    case 3: {
      enterOuterAlt(_localctx, 3);
      setState(327);
      unaryOp();
      setState(328);
      unaryExp();
      break;
    }

    default:
      break;
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- UnaryOpContext ------------------------------------------------------------------

SysYParser::UnaryOpContext::UnaryOpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::UnaryOpContext::ADD() {
  return getToken(SysYParser::ADD, 0);
}

tree::TerminalNode* SysYParser::UnaryOpContext::SUB() {
  return getToken(SysYParser::SUB, 0);
}

tree::TerminalNode* SysYParser::UnaryOpContext::NOT() {
  return getToken(SysYParser::NOT, 0);
}


size_t SysYParser::UnaryOpContext::getRuleIndex() const {
  return SysYParser::RuleUnaryOp;
}


antlrcpp::Any SysYParser::UnaryOpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitUnaryOp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::UnaryOpContext* SysYParser::unaryOp() {
  UnaryOpContext *_localctx = _tracker.createInstance<UnaryOpContext>(_ctx, getState());
  enterRule(_localctx, 50, SysYParser::RuleUnaryOp);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(332);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::ADD)
      | (1ULL << SysYParser::SUB)
      | (1ULL << SysYParser::NOT))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FuncRParamsContext ------------------------------------------------------------------

SysYParser::FuncRParamsContext::FuncRParamsContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

std::vector<SysYParser::ExpContext *> SysYParser::FuncRParamsContext::exp() {
  return getRuleContexts<SysYParser::ExpContext>();
}

SysYParser::ExpContext* SysYParser::FuncRParamsContext::exp(size_t i) {
  return getRuleContext<SysYParser::ExpContext>(i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncRParamsContext::STRING() {
  return getTokens(SysYParser::STRING);
}

tree::TerminalNode* SysYParser::FuncRParamsContext::STRING(size_t i) {
  return getToken(SysYParser::STRING, i);
}

std::vector<tree::TerminalNode *> SysYParser::FuncRParamsContext::COMMA() {
  return getTokens(SysYParser::COMMA);
}

tree::TerminalNode* SysYParser::FuncRParamsContext::COMMA(size_t i) {
  return getToken(SysYParser::COMMA, i);
}


size_t SysYParser::FuncRParamsContext::getRuleIndex() const {
  return SysYParser::RuleFuncRParams;
}


antlrcpp::Any SysYParser::FuncRParamsContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFuncRParams(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FuncRParamsContext* SysYParser::funcRParams() {
  FuncRParamsContext *_localctx = _tracker.createInstance<FuncRParamsContext>(_ctx, getState());
  enterRule(_localctx, 52, SysYParser::RuleFuncRParams);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(336);
    _errHandler->sync(this);
    switch (_input->LA(1)) {
      case SysYParser::ADD:
      case SysYParser::SUB:
      case SysYParser::NOT:
      case SysYParser::Ident:
      case SysYParser::DEC:
      case SysYParser::OCT:
      case SysYParser::HEX:
      case SysYParser::DECFLOAT:
      case SysYParser::HEXFLOAT:
      case SysYParser::LBRA: {
        setState(334);
        exp();
        break;
      }

      case SysYParser::STRING: {
        setState(335);
        match(SysYParser::STRING);
        break;
      }

    default:
      throw NoViableAltException(this);
    }
    setState(345);
    _errHandler->sync(this);
    _la = _input->LA(1);
    while (_la == SysYParser::COMMA) {
      setState(338);
      match(SysYParser::COMMA);
      setState(341);
      _errHandler->sync(this);
      switch (_input->LA(1)) {
        case SysYParser::ADD:
        case SysYParser::SUB:
        case SysYParser::NOT:
        case SysYParser::Ident:
        case SysYParser::DEC:
        case SysYParser::OCT:
        case SysYParser::HEX:
        case SysYParser::DECFLOAT:
        case SysYParser::HEXFLOAT:
        case SysYParser::LBRA: {
          setState(339);
          exp();
          break;
        }

        case SysYParser::STRING: {
          setState(340);
          match(SysYParser::STRING);
          break;
        }

      default:
        throw NoViableAltException(this);
      }
      setState(347);
      _errHandler->sync(this);
      _la = _input->LA(1);
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- MulExpContext ------------------------------------------------------------------

SysYParser::MulExpContext::MulExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::UnaryExpContext* SysYParser::MulExpContext::unaryExp() {
  return getRuleContext<SysYParser::UnaryExpContext>(0);
}

SysYParser::MulExpContext* SysYParser::MulExpContext::mulExp() {
  return getRuleContext<SysYParser::MulExpContext>(0);
}

tree::TerminalNode* SysYParser::MulExpContext::MUL() {
  return getToken(SysYParser::MUL, 0);
}

tree::TerminalNode* SysYParser::MulExpContext::DIV() {
  return getToken(SysYParser::DIV, 0);
}

tree::TerminalNode* SysYParser::MulExpContext::MOD() {
  return getToken(SysYParser::MOD, 0);
}


size_t SysYParser::MulExpContext::getRuleIndex() const {
  return SysYParser::RuleMulExp;
}


antlrcpp::Any SysYParser::MulExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitMulExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::MulExpContext* SysYParser::mulExp() {
   return mulExp(0);
}

SysYParser::MulExpContext* SysYParser::mulExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::MulExpContext *_localctx = _tracker.createInstance<MulExpContext>(_ctx, parentState);
  SysYParser::MulExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 54;
  enterRecursionRule(_localctx, 54, SysYParser::RuleMulExp, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(349);
    unaryExp();
    _ctx->stop = _input->LT(-1);
    setState(356);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<MulExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleMulExp);
        setState(351);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(352);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::MUL)
          | (1ULL << SysYParser::DIV)
          | (1ULL << SysYParser::MOD))) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(353);
        unaryExp(); 
      }
      setState(358);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 37, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- AddExpContext ------------------------------------------------------------------

SysYParser::AddExpContext::AddExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::MulExpContext* SysYParser::AddExpContext::mulExp() {
  return getRuleContext<SysYParser::MulExpContext>(0);
}

SysYParser::AddExpContext* SysYParser::AddExpContext::addExp() {
  return getRuleContext<SysYParser::AddExpContext>(0);
}

tree::TerminalNode* SysYParser::AddExpContext::ADD() {
  return getToken(SysYParser::ADD, 0);
}

tree::TerminalNode* SysYParser::AddExpContext::SUB() {
  return getToken(SysYParser::SUB, 0);
}


size_t SysYParser::AddExpContext::getRuleIndex() const {
  return SysYParser::RuleAddExp;
}


antlrcpp::Any SysYParser::AddExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitAddExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::AddExpContext* SysYParser::addExp() {
   return addExp(0);
}

SysYParser::AddExpContext* SysYParser::addExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::AddExpContext *_localctx = _tracker.createInstance<AddExpContext>(_ctx, parentState);
  SysYParser::AddExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 56;
  enterRecursionRule(_localctx, 56, SysYParser::RuleAddExp, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(360);
    mulExp(0);
    _ctx->stop = _input->LT(-1);
    setState(367);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<AddExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleAddExp);
        setState(362);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(363);
        _la = _input->LA(1);
        if (!(_la == SysYParser::ADD

        || _la == SysYParser::SUB)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(364);
        mulExp(0); 
      }
      setState(369);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 38, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- RelExpContext ------------------------------------------------------------------

SysYParser::RelExpContext::RelExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AddExpContext* SysYParser::RelExpContext::addExp() {
  return getRuleContext<SysYParser::AddExpContext>(0);
}

SysYParser::RelExpContext* SysYParser::RelExpContext::relExp() {
  return getRuleContext<SysYParser::RelExpContext>(0);
}

tree::TerminalNode* SysYParser::RelExpContext::LES() {
  return getToken(SysYParser::LES, 0);
}

tree::TerminalNode* SysYParser::RelExpContext::GRT() {
  return getToken(SysYParser::GRT, 0);
}

tree::TerminalNode* SysYParser::RelExpContext::LEQ() {
  return getToken(SysYParser::LEQ, 0);
}

tree::TerminalNode* SysYParser::RelExpContext::GEQ() {
  return getToken(SysYParser::GEQ, 0);
}


size_t SysYParser::RelExpContext::getRuleIndex() const {
  return SysYParser::RuleRelExp;
}


antlrcpp::Any SysYParser::RelExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitRelExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::RelExpContext* SysYParser::relExp() {
   return relExp(0);
}

SysYParser::RelExpContext* SysYParser::relExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::RelExpContext *_localctx = _tracker.createInstance<RelExpContext>(_ctx, parentState);
  SysYParser::RelExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 58;
  enterRecursionRule(_localctx, 58, SysYParser::RuleRelExp, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(371);
    addExp(0);
    _ctx->stop = _input->LT(-1);
    setState(378);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<RelExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleRelExp);
        setState(373);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(374);
        _la = _input->LA(1);
        if (!((((_la & ~ 0x3fULL) == 0) &&
          ((1ULL << _la) & ((1ULL << SysYParser::LES)
          | (1ULL << SysYParser::GRT)
          | (1ULL << SysYParser::LEQ)
          | (1ULL << SysYParser::GEQ))) != 0))) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(375);
        addExp(0); 
      }
      setState(380);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 39, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- EqExpContext ------------------------------------------------------------------

SysYParser::EqExpContext::EqExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::RelExpContext* SysYParser::EqExpContext::relExp() {
  return getRuleContext<SysYParser::RelExpContext>(0);
}

SysYParser::EqExpContext* SysYParser::EqExpContext::eqExp() {
  return getRuleContext<SysYParser::EqExpContext>(0);
}

tree::TerminalNode* SysYParser::EqExpContext::EQL() {
  return getToken(SysYParser::EQL, 0);
}

tree::TerminalNode* SysYParser::EqExpContext::NEQ() {
  return getToken(SysYParser::NEQ, 0);
}


size_t SysYParser::EqExpContext::getRuleIndex() const {
  return SysYParser::RuleEqExp;
}


antlrcpp::Any SysYParser::EqExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitEqExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::EqExpContext* SysYParser::eqExp() {
   return eqExp(0);
}

SysYParser::EqExpContext* SysYParser::eqExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::EqExpContext *_localctx = _tracker.createInstance<EqExpContext>(_ctx, parentState);
  SysYParser::EqExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 60;
  enterRecursionRule(_localctx, 60, SysYParser::RuleEqExp, precedence);

    size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(382);
    relExp(0);
    _ctx->stop = _input->LT(-1);
    setState(389);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<EqExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleEqExp);
        setState(384);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(385);
        _la = _input->LA(1);
        if (!(_la == SysYParser::EQL

        || _la == SysYParser::NEQ)) {
        _errHandler->recoverInline(this);
        }
        else {
          _errHandler->reportMatch(this);
          consume();
        }
        setState(386);
        relExp(0); 
      }
      setState(391);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 40, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- LAndExpContext ------------------------------------------------------------------

SysYParser::LAndExpContext::LAndExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::EqExpContext* SysYParser::LAndExpContext::eqExp() {
  return getRuleContext<SysYParser::EqExpContext>(0);
}

SysYParser::LAndExpContext* SysYParser::LAndExpContext::lAndExp() {
  return getRuleContext<SysYParser::LAndExpContext>(0);
}

tree::TerminalNode* SysYParser::LAndExpContext::AND() {
  return getToken(SysYParser::AND, 0);
}


size_t SysYParser::LAndExpContext::getRuleIndex() const {
  return SysYParser::RuleLAndExp;
}


antlrcpp::Any SysYParser::LAndExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLAndExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::LAndExpContext* SysYParser::lAndExp() {
   return lAndExp(0);
}

SysYParser::LAndExpContext* SysYParser::lAndExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::LAndExpContext *_localctx = _tracker.createInstance<LAndExpContext>(_ctx, parentState);
  SysYParser::LAndExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 62;
  enterRecursionRule(_localctx, 62, SysYParser::RuleLAndExp, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(393);
    eqExp(0);
    _ctx->stop = _input->LT(-1);
    setState(400);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<LAndExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleLAndExp);
        setState(395);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(396);
        match(SysYParser::AND);
        setState(397);
        eqExp(0); 
      }
      setState(402);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 41, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- LOrExpContext ------------------------------------------------------------------

SysYParser::LOrExpContext::LOrExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::LAndExpContext* SysYParser::LOrExpContext::lAndExp() {
  return getRuleContext<SysYParser::LAndExpContext>(0);
}

SysYParser::LOrExpContext* SysYParser::LOrExpContext::lOrExp() {
  return getRuleContext<SysYParser::LOrExpContext>(0);
}

tree::TerminalNode* SysYParser::LOrExpContext::OR() {
  return getToken(SysYParser::OR, 0);
}


size_t SysYParser::LOrExpContext::getRuleIndex() const {
  return SysYParser::RuleLOrExp;
}


antlrcpp::Any SysYParser::LOrExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitLOrExp(this);
  else
    return visitor->visitChildren(this);
}


SysYParser::LOrExpContext* SysYParser::lOrExp() {
   return lOrExp(0);
}

SysYParser::LOrExpContext* SysYParser::lOrExp(int precedence) {
  ParserRuleContext *parentContext = _ctx;
  size_t parentState = getState();
  SysYParser::LOrExpContext *_localctx = _tracker.createInstance<LOrExpContext>(_ctx, parentState);
  SysYParser::LOrExpContext *previousContext = _localctx;
  (void)previousContext; // Silence compiler, in case the context is not used by generated code.
  size_t startState = 64;
  enterRecursionRule(_localctx, 64, SysYParser::RuleLOrExp, precedence);

    

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    unrollRecursionContexts(parentContext);
  });
  try {
    size_t alt;
    enterOuterAlt(_localctx, 1);
    setState(404);
    lAndExp(0);
    _ctx->stop = _input->LT(-1);
    setState(411);
    _errHandler->sync(this);
    alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    while (alt != 2 && alt != atn::ATN::INVALID_ALT_NUMBER) {
      if (alt == 1) {
        if (!_parseListeners.empty())
          triggerExitRuleEvent();
        previousContext = _localctx;
        _localctx = _tracker.createInstance<LOrExpContext>(parentContext, parentState);
        pushNewRecursionContext(_localctx, startState, RuleLOrExp);
        setState(406);

        if (!(precpred(_ctx, 1))) throw FailedPredicateException(this, "precpred(_ctx, 1)");
        setState(407);
        match(SysYParser::OR);
        setState(408);
        lAndExp(0); 
      }
      setState(413);
      _errHandler->sync(this);
      alt = getInterpreter<atn::ParserATNSimulator>()->adaptivePredict(_input, 42, _ctx);
    }
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }
  return _localctx;
}

//----------------- ConstExpContext ------------------------------------------------------------------

SysYParser::ConstExpContext::ConstExpContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

SysYParser::AddExpContext* SysYParser::ConstExpContext::addExp() {
  return getRuleContext<SysYParser::AddExpContext>(0);
}


size_t SysYParser::ConstExpContext::getRuleIndex() const {
  return SysYParser::RuleConstExp;
}


antlrcpp::Any SysYParser::ConstExpContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitConstExp(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::ConstExpContext* SysYParser::constExp() {
  ConstExpContext *_localctx = _tracker.createInstance<ConstExpContext>(_ctx, getState());
  enterRule(_localctx, 66, SysYParser::RuleConstExp);

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(414);
    addExp(0);
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- IntConstContext ------------------------------------------------------------------

SysYParser::IntConstContext::IntConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::IntConstContext::DEC() {
  return getToken(SysYParser::DEC, 0);
}

tree::TerminalNode* SysYParser::IntConstContext::OCT() {
  return getToken(SysYParser::OCT, 0);
}

tree::TerminalNode* SysYParser::IntConstContext::HEX() {
  return getToken(SysYParser::HEX, 0);
}


size_t SysYParser::IntConstContext::getRuleIndex() const {
  return SysYParser::RuleIntConst;
}


antlrcpp::Any SysYParser::IntConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitIntConst(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::IntConstContext* SysYParser::intConst() {
  IntConstContext *_localctx = _tracker.createInstance<IntConstContext>(_ctx, getState());
  enterRule(_localctx, 68, SysYParser::RuleIntConst);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(416);
    _la = _input->LA(1);
    if (!((((_la & ~ 0x3fULL) == 0) &&
      ((1ULL << _la) & ((1ULL << SysYParser::DEC)
      | (1ULL << SysYParser::OCT)
      | (1ULL << SysYParser::HEX))) != 0))) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

//----------------- FloatConstContext ------------------------------------------------------------------

SysYParser::FloatConstContext::FloatConstContext(ParserRuleContext *parent, size_t invokingState)
  : ParserRuleContext(parent, invokingState) {
}

tree::TerminalNode* SysYParser::FloatConstContext::DECFLOAT() {
  return getToken(SysYParser::DECFLOAT, 0);
}

tree::TerminalNode* SysYParser::FloatConstContext::HEXFLOAT() {
  return getToken(SysYParser::HEXFLOAT, 0);
}


size_t SysYParser::FloatConstContext::getRuleIndex() const {
  return SysYParser::RuleFloatConst;
}


antlrcpp::Any SysYParser::FloatConstContext::accept(tree::ParseTreeVisitor *visitor) {
  if (auto parserVisitor = dynamic_cast<SysYParserVisitor*>(visitor))
    return parserVisitor->visitFloatConst(this);
  else
    return visitor->visitChildren(this);
}

SysYParser::FloatConstContext* SysYParser::floatConst() {
  FloatConstContext *_localctx = _tracker.createInstance<FloatConstContext>(_ctx, getState());
  enterRule(_localctx, 70, SysYParser::RuleFloatConst);
  size_t _la = 0;

#if __cplusplus > 201703L
  auto onExit = finally([=, this] {
#else
  auto onExit = finally([=] {
#endif
    exitRule();
  });
  try {
    enterOuterAlt(_localctx, 1);
    setState(418);
    _la = _input->LA(1);
    if (!(_la == SysYParser::DECFLOAT

    || _la == SysYParser::HEXFLOAT)) {
    _errHandler->recoverInline(this);
    }
    else {
      _errHandler->reportMatch(this);
      consume();
    }
   
  }
  catch (RecognitionException &e) {
    _errHandler->reportError(this, e);
    _localctx->exception = std::current_exception();
    _errHandler->recover(this, _localctx->exception);
  }

  return _localctx;
}

bool SysYParser::sempred(RuleContext *context, size_t ruleIndex, size_t predicateIndex) {
  switch (ruleIndex) {
    case 27: return mulExpSempred(dynamic_cast<MulExpContext *>(context), predicateIndex);
    case 28: return addExpSempred(dynamic_cast<AddExpContext *>(context), predicateIndex);
    case 29: return relExpSempred(dynamic_cast<RelExpContext *>(context), predicateIndex);
    case 30: return eqExpSempred(dynamic_cast<EqExpContext *>(context), predicateIndex);
    case 31: return lAndExpSempred(dynamic_cast<LAndExpContext *>(context), predicateIndex);
    case 32: return lOrExpSempred(dynamic_cast<LOrExpContext *>(context), predicateIndex);

  default:
    break;
  }
  return true;
}

bool SysYParser::mulExpSempred(MulExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 0: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SysYParser::addExpSempred(AddExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 1: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SysYParser::relExpSempred(RelExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 2: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SysYParser::eqExpSempred(EqExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 3: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SysYParser::lAndExpSempred(LAndExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 4: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

bool SysYParser::lOrExpSempred(LOrExpContext *_localctx, size_t predicateIndex) {
  switch (predicateIndex) {
    case 5: return precpred(_ctx, 1);

  default:
    break;
  }
  return true;
}

// Static vars and initialization.
std::vector<dfa::DFA> SysYParser::_decisionToDFA;
atn::PredictionContextCache SysYParser::_sharedContextCache;

// We own the ATN which in turn owns the ATN states.
atn::ATN SysYParser::_atn;
std::vector<uint16_t> SysYParser::_serializedATN;

std::vector<std::string> SysYParser::_ruleNames = {
  "compUnit", "element", "decl", "constDecl", "bType", "constDef", "constInitVal", 
  "varDecl", "varDecl_in", "varDef", "initVal", "funcDef", "funcType", "funcFParams", 
  "funcFParam", "block", "blockItem", "stmt_in", "stmt", "exp", "cond", 
  "lVal", "primaryExp", "number", "unaryExp", "unaryOp", "funcRParams", 
  "mulExp", "addExp", "relExp", "eqExp", "lAndExp", "lOrExp", "constExp", 
  "intConst", "floatConst"
};

std::vector<std::string> SysYParser::_literalNames = {
  "", "'int'", "'float'", "'const'", "'void'", "", "", "'if'", "'else'", 
  "'while'", "'for'", "'break'", "'continue'", "'return'", "'='", "'+'", 
  "'-'", "'*'", "'/'", "'%'", "'=='", "'!='", "'<'", "'>'", "'<='", "'>='", 
  "'!'", "'&&'", "'||'", "", "", "", "", "", "", "','", "';'", "'('", "')'", 
  "'['", "']'", "'{'", "'}'"
};

std::vector<std::string> SysYParser::_symbolicNames = {
  "", "INT", "FLOAT", "CONST", "VOID", "STRING", "ESC", "IF", "ELSE", "WHILE", 
  "FOR", "BREAK", "CONTINUE", "RETURN", "AGN", "ADD", "SUB", "MUL", "DIV", 
  "MOD", "EQL", "NEQ", "LES", "GRT", "LEQ", "GEQ", "NOT", "AND", "OR", "Ident", 
  "DEC", "OCT", "HEX", "DECFLOAT", "HEXFLOAT", "COMMA", "SEMI", "LBRA", 
  "RBRA", "LSBRA", "RSBRA", "LCBRA", "RCBRA", "WS", "SL_COMMENT", "COMMENT"
};

dfa::Vocabulary SysYParser::_vocabulary(_literalNames, _symbolicNames);

std::vector<std::string> SysYParser::_tokenNames;

SysYParser::Initializer::Initializer() {
	for (size_t i = 0; i < _symbolicNames.size(); ++i) {
		std::string name = _vocabulary.getLiteralName(i);
		if (name.empty()) {
			name = _vocabulary.getSymbolicName(i);
		}

		if (name.empty()) {
			_tokenNames.push_back("<INVALID>");
		} else {
      _tokenNames.push_back(name);
    }
	}

  static const uint16_t serializedATNSegment0[] = {
    0x3, 0x608b, 0xa72a, 0x8133, 0xb9ed, 0x417c, 0x3be7, 0x7786, 0x5964, 
       0x3, 0x2f, 0x1a7, 0x4, 0x2, 0x9, 0x2, 0x4, 0x3, 0x9, 0x3, 0x4, 0x4, 
       0x9, 0x4, 0x4, 0x5, 0x9, 0x5, 0x4, 0x6, 0x9, 0x6, 0x4, 0x7, 0x9, 
       0x7, 0x4, 0x8, 0x9, 0x8, 0x4, 0x9, 0x9, 0x9, 0x4, 0xa, 0x9, 0xa, 
       0x4, 0xb, 0x9, 0xb, 0x4, 0xc, 0x9, 0xc, 0x4, 0xd, 0x9, 0xd, 0x4, 
       0xe, 0x9, 0xe, 0x4, 0xf, 0x9, 0xf, 0x4, 0x10, 0x9, 0x10, 0x4, 0x11, 
       0x9, 0x11, 0x4, 0x12, 0x9, 0x12, 0x4, 0x13, 0x9, 0x13, 0x4, 0x14, 
       0x9, 0x14, 0x4, 0x15, 0x9, 0x15, 0x4, 0x16, 0x9, 0x16, 0x4, 0x17, 
       0x9, 0x17, 0x4, 0x18, 0x9, 0x18, 0x4, 0x19, 0x9, 0x19, 0x4, 0x1a, 
       0x9, 0x1a, 0x4, 0x1b, 0x9, 0x1b, 0x4, 0x1c, 0x9, 0x1c, 0x4, 0x1d, 
       0x9, 0x1d, 0x4, 0x1e, 0x9, 0x1e, 0x4, 0x1f, 0x9, 0x1f, 0x4, 0x20, 
       0x9, 0x20, 0x4, 0x21, 0x9, 0x21, 0x4, 0x22, 0x9, 0x22, 0x4, 0x23, 
       0x9, 0x23, 0x4, 0x24, 0x9, 0x24, 0x4, 0x25, 0x9, 0x25, 0x3, 0x2, 
       0x7, 0x2, 0x4c, 0xa, 0x2, 0xc, 0x2, 0xe, 0x2, 0x4f, 0xb, 0x2, 0x3, 
       0x2, 0x3, 0x2, 0x3, 0x3, 0x3, 0x3, 0x5, 0x3, 0x55, 0xa, 0x3, 0x3, 
       0x4, 0x3, 0x4, 0x5, 0x4, 0x59, 0xa, 0x4, 0x3, 0x5, 0x3, 0x5, 0x3, 
       0x5, 0x3, 0x5, 0x3, 0x5, 0x7, 0x5, 0x60, 0xa, 0x5, 0xc, 0x5, 0xe, 
       0x5, 0x63, 0xb, 0x5, 0x3, 0x5, 0x3, 0x5, 0x3, 0x6, 0x3, 0x6, 0x3, 
       0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 0x7, 0x7, 0x7, 0x6e, 0xa, 
       0x7, 0xc, 0x7, 0xe, 0x7, 0x71, 0xb, 0x7, 0x3, 0x7, 0x3, 0x7, 0x3, 
       0x7, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x3, 0x8, 0x7, 0x8, 
       0x7b, 0xa, 0x8, 0xc, 0x8, 0xe, 0x8, 0x7e, 0xb, 0x8, 0x5, 0x8, 0x80, 
       0xa, 0x8, 0x3, 0x8, 0x5, 0x8, 0x83, 0xa, 0x8, 0x3, 0x9, 0x3, 0x9, 
       0x3, 0x9, 0x3, 0x9, 0x7, 0x9, 0x89, 0xa, 0x9, 0xc, 0x9, 0xe, 0x9, 
       0x8c, 0xb, 0x9, 0x3, 0x9, 0x3, 0x9, 0x3, 0xa, 0x3, 0xa, 0x3, 0xa, 
       0x3, 0xa, 0x7, 0xa, 0x94, 0xa, 0xa, 0xc, 0xa, 0xe, 0xa, 0x97, 0xb, 
       0xa, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 
       0x9e, 0xa, 0xb, 0xc, 0xb, 0xe, 0xb, 0xa1, 0xb, 0xb, 0x3, 0xb, 0x3, 
       0xb, 0x3, 0xb, 0x3, 0xb, 0x3, 0xb, 0x7, 0xb, 0xa8, 0xa, 0xb, 0xc, 
       0xb, 0xe, 0xb, 0xab, 0xb, 0xb, 0x3, 0xb, 0x3, 0xb, 0x5, 0xb, 0xaf, 
       0xa, 0xb, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x3, 0xc, 0x7, 
       0xc, 0xb6, 0xa, 0xc, 0xc, 0xc, 0xe, 0xc, 0xb9, 0xb, 0xc, 0x5, 0xc, 
       0xbb, 0xa, 0xc, 0x3, 0xc, 0x5, 0xc, 0xbe, 0xa, 0xc, 0x3, 0xd, 0x3, 
       0xd, 0x3, 0xd, 0x3, 0xd, 0x5, 0xd, 0xc4, 0xa, 0xd, 0x3, 0xd, 0x3, 
       0xd, 0x3, 0xd, 0x3, 0xe, 0x3, 0xe, 0x3, 0xf, 0x3, 0xf, 0x3, 0xf, 
       0x7, 0xf, 0xce, 0xa, 0xf, 0xc, 0xf, 0xe, 0xf, 0xd1, 0xb, 0xf, 0x3, 
       0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 0x10, 0x3, 
       0x10, 0x3, 0x10, 0x7, 0x10, 0xdb, 0xa, 0x10, 0xc, 0x10, 0xe, 0x10, 
       0xde, 0xb, 0x10, 0x5, 0x10, 0xe0, 0xa, 0x10, 0x3, 0x11, 0x3, 0x11, 
       0x7, 0x11, 0xe4, 0xa, 0x11, 0xc, 0x11, 0xe, 0x11, 0xe7, 0xb, 0x11, 
       0x3, 0x11, 0x3, 0x11, 0x3, 0x12, 0x3, 0x12, 0x5, 0x12, 0xed, 0xa, 
       0x12, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x3, 0x13, 0x5, 
       0x13, 0xf4, 0xa, 0x13, 0x5, 0x13, 0xf6, 0xa, 0x13, 0x3, 0x14, 0x3, 
       0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0xfe, 
       0xa, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
       0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x109, 0xa, 
       0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x10f, 
       0xa, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
       0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
       0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 0x3, 0x14, 
       0x3, 0x14, 0x3, 0x14, 0x5, 0x14, 0x124, 0xa, 0x14, 0x3, 0x14, 0x5, 
       0x14, 0x127, 0xa, 0x14, 0x3, 0x15, 0x3, 0x15, 0x3, 0x16, 0x3, 0x16, 
       0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x3, 0x17, 0x7, 0x17, 
       0x132, 0xa, 0x17, 0xc, 0x17, 0xe, 0x17, 0x135, 0xb, 0x17, 0x3, 0x18, 
       0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x3, 0x18, 0x5, 0x18, 
       0x13d, 0xa, 0x18, 0x3, 0x19, 0x3, 0x19, 0x5, 0x19, 0x141, 0xa, 0x19, 
       0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x147, 0xa, 
       0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x3, 0x1a, 0x5, 0x1a, 0x14d, 
       0xa, 0x1a, 0x3, 0x1b, 0x3, 0x1b, 0x3, 0x1c, 0x3, 0x1c, 0x5, 0x1c, 
       0x153, 0xa, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x3, 0x1c, 0x5, 0x1c, 0x158, 
       0xa, 0x1c, 0x7, 0x1c, 0x15a, 0xa, 0x1c, 0xc, 0x1c, 0xe, 0x1c, 0x15d, 
       0xb, 0x1c, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 0x3, 0x1d, 
       0x3, 0x1d, 0x7, 0x1d, 0x165, 0xa, 0x1d, 0xc, 0x1d, 0xe, 0x1d, 0x168, 
       0xb, 0x1d, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 0x3, 0x1e, 
       0x3, 0x1e, 0x7, 0x1e, 0x170, 0xa, 0x1e, 0xc, 0x1e, 0xe, 0x1e, 0x173, 
       0xb, 0x1e, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 0x3, 0x1f, 
       0x3, 0x1f, 0x7, 0x1f, 0x17b, 0xa, 0x1f, 0xc, 0x1f, 0xe, 0x1f, 0x17e, 
       0xb, 0x1f, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 0x3, 0x20, 
       0x3, 0x20, 0x7, 0x20, 0x186, 0xa, 0x20, 0xc, 0x20, 0xe, 0x20, 0x189, 
       0xb, 0x20, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 0x3, 0x21, 
       0x3, 0x21, 0x7, 0x21, 0x191, 0xa, 0x21, 0xc, 0x21, 0xe, 0x21, 0x194, 
       0xb, 0x21, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 0x3, 0x22, 
       0x3, 0x22, 0x7, 0x22, 0x19c, 0xa, 0x22, 0xc, 0x22, 0xe, 0x22, 0x19f, 
       0xb, 0x22, 0x3, 0x23, 0x3, 0x23, 0x3, 0x24, 0x3, 0x24, 0x3, 0x25, 
       0x3, 0x25, 0x3, 0x25, 0x2, 0x8, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 
       0x26, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 
       0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 
       0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 
       0x44, 0x46, 0x48, 0x2, 0xb, 0x3, 0x2, 0x3, 0x4, 0x4, 0x2, 0x3, 0x4, 
       0x6, 0x6, 0x4, 0x2, 0x11, 0x12, 0x1c, 0x1c, 0x3, 0x2, 0x13, 0x15, 
       0x3, 0x2, 0x11, 0x12, 0x3, 0x2, 0x18, 0x1b, 0x3, 0x2, 0x16, 0x17, 
       0x3, 0x2, 0x20, 0x22, 0x3, 0x2, 0x23, 0x24, 0x2, 0x1b6, 0x2, 0x4d, 
       0x3, 0x2, 0x2, 0x2, 0x4, 0x54, 0x3, 0x2, 0x2, 0x2, 0x6, 0x58, 0x3, 
       0x2, 0x2, 0x2, 0x8, 0x5a, 0x3, 0x2, 0x2, 0x2, 0xa, 0x66, 0x3, 0x2, 
       0x2, 0x2, 0xc, 0x68, 0x3, 0x2, 0x2, 0x2, 0xe, 0x82, 0x3, 0x2, 0x2, 
       0x2, 0x10, 0x84, 0x3, 0x2, 0x2, 0x2, 0x12, 0x8f, 0x3, 0x2, 0x2, 0x2, 
       0x14, 0xae, 0x3, 0x2, 0x2, 0x2, 0x16, 0xbd, 0x3, 0x2, 0x2, 0x2, 0x18, 
       0xbf, 0x3, 0x2, 0x2, 0x2, 0x1a, 0xc8, 0x3, 0x2, 0x2, 0x2, 0x1c, 0xca, 
       0x3, 0x2, 0x2, 0x2, 0x1e, 0xd2, 0x3, 0x2, 0x2, 0x2, 0x20, 0xe1, 0x3, 
       0x2, 0x2, 0x2, 0x22, 0xec, 0x3, 0x2, 0x2, 0x2, 0x24, 0xf5, 0x3, 0x2, 
       0x2, 0x2, 0x26, 0x126, 0x3, 0x2, 0x2, 0x2, 0x28, 0x128, 0x3, 0x2, 
       0x2, 0x2, 0x2a, 0x12a, 0x3, 0x2, 0x2, 0x2, 0x2c, 0x12c, 0x3, 0x2, 
       0x2, 0x2, 0x2e, 0x13c, 0x3, 0x2, 0x2, 0x2, 0x30, 0x140, 0x3, 0x2, 
       0x2, 0x2, 0x32, 0x14c, 0x3, 0x2, 0x2, 0x2, 0x34, 0x14e, 0x3, 0x2, 
       0x2, 0x2, 0x36, 0x152, 0x3, 0x2, 0x2, 0x2, 0x38, 0x15e, 0x3, 0x2, 
       0x2, 0x2, 0x3a, 0x169, 0x3, 0x2, 0x2, 0x2, 0x3c, 0x174, 0x3, 0x2, 
       0x2, 0x2, 0x3e, 0x17f, 0x3, 0x2, 0x2, 0x2, 0x40, 0x18a, 0x3, 0x2, 
       0x2, 0x2, 0x42, 0x195, 0x3, 0x2, 0x2, 0x2, 0x44, 0x1a0, 0x3, 0x2, 
       0x2, 0x2, 0x46, 0x1a2, 0x3, 0x2, 0x2, 0x2, 0x48, 0x1a4, 0x3, 0x2, 
       0x2, 0x2, 0x4a, 0x4c, 0x5, 0x4, 0x3, 0x2, 0x4b, 0x4a, 0x3, 0x2, 0x2, 
       0x2, 0x4c, 0x4f, 0x3, 0x2, 0x2, 0x2, 0x4d, 0x4b, 0x3, 0x2, 0x2, 0x2, 
       0x4d, 0x4e, 0x3, 0x2, 0x2, 0x2, 0x4e, 0x50, 0x3, 0x2, 0x2, 0x2, 0x4f, 
       0x4d, 0x3, 0x2, 0x2, 0x2, 0x50, 0x51, 0x7, 0x2, 0x2, 0x3, 0x51, 0x3, 
       0x3, 0x2, 0x2, 0x2, 0x52, 0x55, 0x5, 0x6, 0x4, 0x2, 0x53, 0x55, 0x5, 
       0x18, 0xd, 0x2, 0x54, 0x52, 0x3, 0x2, 0x2, 0x2, 0x54, 0x53, 0x3, 
       0x2, 0x2, 0x2, 0x55, 0x5, 0x3, 0x2, 0x2, 0x2, 0x56, 0x59, 0x5, 0x8, 
       0x5, 0x2, 0x57, 0x59, 0x5, 0x10, 0x9, 0x2, 0x58, 0x56, 0x3, 0x2, 
       0x2, 0x2, 0x58, 0x57, 0x3, 0x2, 0x2, 0x2, 0x59, 0x7, 0x3, 0x2, 0x2, 
       0x2, 0x5a, 0x5b, 0x7, 0x5, 0x2, 0x2, 0x5b, 0x5c, 0x5, 0xa, 0x6, 0x2, 
       0x5c, 0x61, 0x5, 0xc, 0x7, 0x2, 0x5d, 0x5e, 0x7, 0x25, 0x2, 0x2, 
       0x5e, 0x60, 0x5, 0xc, 0x7, 0x2, 0x5f, 0x5d, 0x3, 0x2, 0x2, 0x2, 0x60, 
       0x63, 0x3, 0x2, 0x2, 0x2, 0x61, 0x5f, 0x3, 0x2, 0x2, 0x2, 0x61, 0x62, 
       0x3, 0x2, 0x2, 0x2, 0x62, 0x64, 0x3, 0x2, 0x2, 0x2, 0x63, 0x61, 0x3, 
       0x2, 0x2, 0x2, 0x64, 0x65, 0x7, 0x26, 0x2, 0x2, 0x65, 0x9, 0x3, 0x2, 
       0x2, 0x2, 0x66, 0x67, 0x9, 0x2, 0x2, 0x2, 0x67, 0xb, 0x3, 0x2, 0x2, 
       0x2, 0x68, 0x6f, 0x7, 0x1f, 0x2, 0x2, 0x69, 0x6a, 0x7, 0x29, 0x2, 
       0x2, 0x6a, 0x6b, 0x5, 0x44, 0x23, 0x2, 0x6b, 0x6c, 0x7, 0x2a, 0x2, 
       0x2, 0x6c, 0x6e, 0x3, 0x2, 0x2, 0x2, 0x6d, 0x69, 0x3, 0x2, 0x2, 0x2, 
       0x6e, 0x71, 0x3, 0x2, 0x2, 0x2, 0x6f, 0x6d, 0x3, 0x2, 0x2, 0x2, 0x6f, 
       0x70, 0x3, 0x2, 0x2, 0x2, 0x70, 0x72, 0x3, 0x2, 0x2, 0x2, 0x71, 0x6f, 
       0x3, 0x2, 0x2, 0x2, 0x72, 0x73, 0x7, 0x10, 0x2, 0x2, 0x73, 0x74, 
       0x5, 0xe, 0x8, 0x2, 0x74, 0xd, 0x3, 0x2, 0x2, 0x2, 0x75, 0x83, 0x5, 
       0x44, 0x23, 0x2, 0x76, 0x7f, 0x7, 0x2b, 0x2, 0x2, 0x77, 0x7c, 0x5, 
       0xe, 0x8, 0x2, 0x78, 0x79, 0x7, 0x25, 0x2, 0x2, 0x79, 0x7b, 0x5, 
       0xe, 0x8, 0x2, 0x7a, 0x78, 0x3, 0x2, 0x2, 0x2, 0x7b, 0x7e, 0x3, 0x2, 
       0x2, 0x2, 0x7c, 0x7a, 0x3, 0x2, 0x2, 0x2, 0x7c, 0x7d, 0x3, 0x2, 0x2, 
       0x2, 0x7d, 0x80, 0x3, 0x2, 0x2, 0x2, 0x7e, 0x7c, 0x3, 0x2, 0x2, 0x2, 
       0x7f, 0x77, 0x3, 0x2, 0x2, 0x2, 0x7f, 0x80, 0x3, 0x2, 0x2, 0x2, 0x80, 
       0x81, 0x3, 0x2, 0x2, 0x2, 0x81, 0x83, 0x7, 0x2c, 0x2, 0x2, 0x82, 
       0x75, 0x3, 0x2, 0x2, 0x2, 0x82, 0x76, 0x3, 0x2, 0x2, 0x2, 0x83, 0xf, 
       0x3, 0x2, 0x2, 0x2, 0x84, 0x85, 0x5, 0xa, 0x6, 0x2, 0x85, 0x8a, 0x5, 
       0x14, 0xb, 0x2, 0x86, 0x87, 0x7, 0x25, 0x2, 0x2, 0x87, 0x89, 0x5, 
       0x14, 0xb, 0x2, 0x88, 0x86, 0x3, 0x2, 0x2, 0x2, 0x89, 0x8c, 0x3, 
       0x2, 0x2, 0x2, 0x8a, 0x88, 0x3, 0x2, 0x2, 0x2, 0x8a, 0x8b, 0x3, 0x2, 
       0x2, 0x2, 0x8b, 0x8d, 0x3, 0x2, 0x2, 0x2, 0x8c, 0x8a, 0x3, 0x2, 0x2, 
       0x2, 0x8d, 0x8e, 0x7, 0x26, 0x2, 0x2, 0x8e, 0x11, 0x3, 0x2, 0x2, 
       0x2, 0x8f, 0x90, 0x5, 0xa, 0x6, 0x2, 0x90, 0x95, 0x5, 0x14, 0xb, 
       0x2, 0x91, 0x92, 0x7, 0x25, 0x2, 0x2, 0x92, 0x94, 0x5, 0x14, 0xb, 
       0x2, 0x93, 0x91, 0x3, 0x2, 0x2, 0x2, 0x94, 0x97, 0x3, 0x2, 0x2, 0x2, 
       0x95, 0x93, 0x3, 0x2, 0x2, 0x2, 0x95, 0x96, 0x3, 0x2, 0x2, 0x2, 0x96, 
       0x13, 0x3, 0x2, 0x2, 0x2, 0x97, 0x95, 0x3, 0x2, 0x2, 0x2, 0x98, 0x9f, 
       0x7, 0x1f, 0x2, 0x2, 0x99, 0x9a, 0x7, 0x29, 0x2, 0x2, 0x9a, 0x9b, 
       0x5, 0x44, 0x23, 0x2, 0x9b, 0x9c, 0x7, 0x2a, 0x2, 0x2, 0x9c, 0x9e, 
       0x3, 0x2, 0x2, 0x2, 0x9d, 0x99, 0x3, 0x2, 0x2, 0x2, 0x9e, 0xa1, 0x3, 
       0x2, 0x2, 0x2, 0x9f, 0x9d, 0x3, 0x2, 0x2, 0x2, 0x9f, 0xa0, 0x3, 0x2, 
       0x2, 0x2, 0xa0, 0xaf, 0x3, 0x2, 0x2, 0x2, 0xa1, 0x9f, 0x3, 0x2, 0x2, 
       0x2, 0xa2, 0xa9, 0x7, 0x1f, 0x2, 0x2, 0xa3, 0xa4, 0x7, 0x29, 0x2, 
       0x2, 0xa4, 0xa5, 0x5, 0x44, 0x23, 0x2, 0xa5, 0xa6, 0x7, 0x2a, 0x2, 
       0x2, 0xa6, 0xa8, 0x3, 0x2, 0x2, 0x2, 0xa7, 0xa3, 0x3, 0x2, 0x2, 0x2, 
       0xa8, 0xab, 0x3, 0x2, 0x2, 0x2, 0xa9, 0xa7, 0x3, 0x2, 0x2, 0x2, 0xa9, 
       0xaa, 0x3, 0x2, 0x2, 0x2, 0xaa, 0xac, 0x3, 0x2, 0x2, 0x2, 0xab, 0xa9, 
       0x3, 0x2, 0x2, 0x2, 0xac, 0xad, 0x7, 0x10, 0x2, 0x2, 0xad, 0xaf, 
       0x5, 0x16, 0xc, 0x2, 0xae, 0x98, 0x3, 0x2, 0x2, 0x2, 0xae, 0xa2, 
       0x3, 0x2, 0x2, 0x2, 0xaf, 0x15, 0x3, 0x2, 0x2, 0x2, 0xb0, 0xbe, 0x5, 
       0x28, 0x15, 0x2, 0xb1, 0xba, 0x7, 0x2b, 0x2, 0x2, 0xb2, 0xb7, 0x5, 
       0x16, 0xc, 0x2, 0xb3, 0xb4, 0x7, 0x25, 0x2, 0x2, 0xb4, 0xb6, 0x5, 
       0x16, 0xc, 0x2, 0xb5, 0xb3, 0x3, 0x2, 0x2, 0x2, 0xb6, 0xb9, 0x3, 
       0x2, 0x2, 0x2, 0xb7, 0xb5, 0x3, 0x2, 0x2, 0x2, 0xb7, 0xb8, 0x3, 0x2, 
       0x2, 0x2, 0xb8, 0xbb, 0x3, 0x2, 0x2, 0x2, 0xb9, 0xb7, 0x3, 0x2, 0x2, 
       0x2, 0xba, 0xb2, 0x3, 0x2, 0x2, 0x2, 0xba, 0xbb, 0x3, 0x2, 0x2, 0x2, 
       0xbb, 0xbc, 0x3, 0x2, 0x2, 0x2, 0xbc, 0xbe, 0x7, 0x2c, 0x2, 0x2, 
       0xbd, 0xb0, 0x3, 0x2, 0x2, 0x2, 0xbd, 0xb1, 0x3, 0x2, 0x2, 0x2, 0xbe, 
       0x17, 0x3, 0x2, 0x2, 0x2, 0xbf, 0xc0, 0x5, 0x1a, 0xe, 0x2, 0xc0, 
       0xc1, 0x7, 0x1f, 0x2, 0x2, 0xc1, 0xc3, 0x7, 0x27, 0x2, 0x2, 0xc2, 
       0xc4, 0x5, 0x1c, 0xf, 0x2, 0xc3, 0xc2, 0x3, 0x2, 0x2, 0x2, 0xc3, 
       0xc4, 0x3, 0x2, 0x2, 0x2, 0xc4, 0xc5, 0x3, 0x2, 0x2, 0x2, 0xc5, 0xc6, 
       0x7, 0x28, 0x2, 0x2, 0xc6, 0xc7, 0x5, 0x20, 0x11, 0x2, 0xc7, 0x19, 
       0x3, 0x2, 0x2, 0x2, 0xc8, 0xc9, 0x9, 0x3, 0x2, 0x2, 0xc9, 0x1b, 0x3, 
       0x2, 0x2, 0x2, 0xca, 0xcf, 0x5, 0x1e, 0x10, 0x2, 0xcb, 0xcc, 0x7, 
       0x25, 0x2, 0x2, 0xcc, 0xce, 0x5, 0x1e, 0x10, 0x2, 0xcd, 0xcb, 0x3, 
       0x2, 0x2, 0x2, 0xce, 0xd1, 0x3, 0x2, 0x2, 0x2, 0xcf, 0xcd, 0x3, 0x2, 
       0x2, 0x2, 0xcf, 0xd0, 0x3, 0x2, 0x2, 0x2, 0xd0, 0x1d, 0x3, 0x2, 0x2, 
       0x2, 0xd1, 0xcf, 0x3, 0x2, 0x2, 0x2, 0xd2, 0xd3, 0x5, 0xa, 0x6, 0x2, 
       0xd3, 0xdf, 0x7, 0x1f, 0x2, 0x2, 0xd4, 0xd5, 0x7, 0x29, 0x2, 0x2, 
       0xd5, 0xdc, 0x7, 0x2a, 0x2, 0x2, 0xd6, 0xd7, 0x7, 0x29, 0x2, 0x2, 
       0xd7, 0xd8, 0x5, 0x28, 0x15, 0x2, 0xd8, 0xd9, 0x7, 0x2a, 0x2, 0x2, 
       0xd9, 0xdb, 0x3, 0x2, 0x2, 0x2, 0xda, 0xd6, 0x3, 0x2, 0x2, 0x2, 0xdb, 
       0xde, 0x3, 0x2, 0x2, 0x2, 0xdc, 0xda, 0x3, 0x2, 0x2, 0x2, 0xdc, 0xdd, 
       0x3, 0x2, 0x2, 0x2, 0xdd, 0xe0, 0x3, 0x2, 0x2, 0x2, 0xde, 0xdc, 0x3, 
       0x2, 0x2, 0x2, 0xdf, 0xd4, 0x3, 0x2, 0x2, 0x2, 0xdf, 0xe0, 0x3, 0x2, 
       0x2, 0x2, 0xe0, 0x1f, 0x3, 0x2, 0x2, 0x2, 0xe1, 0xe5, 0x7, 0x2b, 
       0x2, 0x2, 0xe2, 0xe4, 0x5, 0x22, 0x12, 0x2, 0xe3, 0xe2, 0x3, 0x2, 
       0x2, 0x2, 0xe4, 0xe7, 0x3, 0x2, 0x2, 0x2, 0xe5, 0xe3, 0x3, 0x2, 0x2, 
       0x2, 0xe5, 0xe6, 0x3, 0x2, 0x2, 0x2, 0xe6, 0xe8, 0x3, 0x2, 0x2, 0x2, 
       0xe7, 0xe5, 0x3, 0x2, 0x2, 0x2, 0xe8, 0xe9, 0x7, 0x2c, 0x2, 0x2, 
       0xe9, 0x21, 0x3, 0x2, 0x2, 0x2, 0xea, 0xed, 0x5, 0x6, 0x4, 0x2, 0xeb, 
       0xed, 0x5, 0x26, 0x14, 0x2, 0xec, 0xea, 0x3, 0x2, 0x2, 0x2, 0xec, 
       0xeb, 0x3, 0x2, 0x2, 0x2, 0xed, 0x23, 0x3, 0x2, 0x2, 0x2, 0xee, 0xef, 
       0x5, 0x2c, 0x17, 0x2, 0xef, 0xf0, 0x7, 0x10, 0x2, 0x2, 0xf0, 0xf1, 
       0x5, 0x28, 0x15, 0x2, 0xf1, 0xf6, 0x3, 0x2, 0x2, 0x2, 0xf2, 0xf4, 
       0x5, 0x28, 0x15, 0x2, 0xf3, 0xf2, 0x3, 0x2, 0x2, 0x2, 0xf3, 0xf4, 
       0x3, 0x2, 0x2, 0x2, 0xf4, 0xf6, 0x3, 0x2, 0x2, 0x2, 0xf5, 0xee, 0x3, 
       0x2, 0x2, 0x2, 0xf5, 0xf3, 0x3, 0x2, 0x2, 0x2, 0xf6, 0x25, 0x3, 0x2, 
       0x2, 0x2, 0xf7, 0xf8, 0x5, 0x2c, 0x17, 0x2, 0xf8, 0xf9, 0x7, 0x10, 
       0x2, 0x2, 0xf9, 0xfa, 0x5, 0x28, 0x15, 0x2, 0xfa, 0xfb, 0x7, 0x26, 
       0x2, 0x2, 0xfb, 0x127, 0x3, 0x2, 0x2, 0x2, 0xfc, 0xfe, 0x5, 0x28, 
       0x15, 0x2, 0xfd, 0xfc, 0x3, 0x2, 0x2, 0x2, 0xfd, 0xfe, 0x3, 0x2, 
       0x2, 0x2, 0xfe, 0xff, 0x3, 0x2, 0x2, 0x2, 0xff, 0x127, 0x7, 0x26, 
       0x2, 0x2, 0x100, 0x127, 0x5, 0x20, 0x11, 0x2, 0x101, 0x102, 0x7, 
       0x9, 0x2, 0x2, 0x102, 0x103, 0x7, 0x27, 0x2, 0x2, 0x103, 0x104, 0x5, 
       0x2a, 0x16, 0x2, 0x104, 0x105, 0x7, 0x28, 0x2, 0x2, 0x105, 0x108, 
       0x5, 0x26, 0x14, 0x2, 0x106, 0x107, 0x7, 0xa, 0x2, 0x2, 0x107, 0x109, 
       0x5, 0x26, 0x14, 0x2, 0x108, 0x106, 0x3, 0x2, 0x2, 0x2, 0x108, 0x109, 
       0x3, 0x2, 0x2, 0x2, 0x109, 0x127, 0x3, 0x2, 0x2, 0x2, 0x10a, 0x10b, 
       0x7, 0xc, 0x2, 0x2, 0x10b, 0x10e, 0x7, 0x27, 0x2, 0x2, 0x10c, 0x10f, 
       0x5, 0x24, 0x13, 0x2, 0x10d, 0x10f, 0x5, 0x12, 0xa, 0x2, 0x10e, 0x10c, 
       0x3, 0x2, 0x2, 0x2, 0x10e, 0x10d, 0x3, 0x2, 0x2, 0x2, 0x10f, 0x110, 
       0x3, 0x2, 0x2, 0x2, 0x110, 0x111, 0x7, 0x26, 0x2, 0x2, 0x111, 0x112, 
       0x5, 0x2a, 0x16, 0x2, 0x112, 0x113, 0x7, 0x26, 0x2, 0x2, 0x113, 0x114, 
       0x5, 0x24, 0x13, 0x2, 0x114, 0x115, 0x7, 0x28, 0x2, 0x2, 0x115, 0x116, 
       0x5, 0x26, 0x14, 0x2, 0x116, 0x127, 0x3, 0x2, 0x2, 0x2, 0x117, 0x118, 
       0x7, 0xb, 0x2, 0x2, 0x118, 0x119, 0x7, 0x27, 0x2, 0x2, 0x119, 0x11a, 
       0x5, 0x2a, 0x16, 0x2, 0x11a, 0x11b, 0x7, 0x28, 0x2, 0x2, 0x11b, 0x11c, 
       0x5, 0x26, 0x14, 0x2, 0x11c, 0x127, 0x3, 0x2, 0x2, 0x2, 0x11d, 0x11e, 
       0x7, 0xd, 0x2, 0x2, 0x11e, 0x127, 0x7, 0x26, 0x2, 0x2, 0x11f, 0x120, 
       0x7, 0xe, 0x2, 0x2, 0x120, 0x127, 0x7, 0x26, 0x2, 0x2, 0x121, 0x123, 
       0x7, 0xf, 0x2, 0x2, 0x122, 0x124, 0x5, 0x28, 0x15, 0x2, 0x123, 0x122, 
       0x3, 0x2, 0x2, 0x2, 0x123, 0x124, 0x3, 0x2, 0x2, 0x2, 0x124, 0x125, 
       0x3, 0x2, 0x2, 0x2, 0x125, 0x127, 0x7, 0x26, 0x2, 0x2, 0x126, 0xf7, 
       0x3, 0x2, 0x2, 0x2, 0x126, 0xfd, 0x3, 0x2, 0x2, 0x2, 0x126, 0x100, 
       0x3, 0x2, 0x2, 0x2, 0x126, 0x101, 0x3, 0x2, 0x2, 0x2, 0x126, 0x10a, 
       0x3, 0x2, 0x2, 0x2, 0x126, 0x117, 0x3, 0x2, 0x2, 0x2, 0x126, 0x11d, 
       0x3, 0x2, 0x2, 0x2, 0x126, 0x11f, 0x3, 0x2, 0x2, 0x2, 0x126, 0x121, 
       0x3, 0x2, 0x2, 0x2, 0x127, 0x27, 0x3, 0x2, 0x2, 0x2, 0x128, 0x129, 
       0x5, 0x3a, 0x1e, 0x2, 0x129, 0x29, 0x3, 0x2, 0x2, 0x2, 0x12a, 0x12b, 
       0x5, 0x42, 0x22, 0x2, 0x12b, 0x2b, 0x3, 0x2, 0x2, 0x2, 0x12c, 0x133, 
       0x7, 0x1f, 0x2, 0x2, 0x12d, 0x12e, 0x7, 0x29, 0x2, 0x2, 0x12e, 0x12f, 
       0x5, 0x28, 0x15, 0x2, 0x12f, 0x130, 0x7, 0x2a, 0x2, 0x2, 0x130, 0x132, 
       0x3, 0x2, 0x2, 0x2, 0x131, 0x12d, 0x3, 0x2, 0x2, 0x2, 0x132, 0x135, 
       0x3, 0x2, 0x2, 0x2, 0x133, 0x131, 0x3, 0x2, 0x2, 0x2, 0x133, 0x134, 
       0x3, 0x2, 0x2, 0x2, 0x134, 0x2d, 0x3, 0x2, 0x2, 0x2, 0x135, 0x133, 
       0x3, 0x2, 0x2, 0x2, 0x136, 0x137, 0x7, 0x27, 0x2, 0x2, 0x137, 0x138, 
       0x5, 0x28, 0x15, 0x2, 0x138, 0x139, 0x7, 0x28, 0x2, 0x2, 0x139, 0x13d, 
       0x3, 0x2, 0x2, 0x2, 0x13a, 0x13d, 0x5, 0x2c, 0x17, 0x2, 0x13b, 0x13d, 
       0x5, 0x30, 0x19, 0x2, 0x13c, 0x136, 0x3, 0x2, 0x2, 0x2, 0x13c, 0x13a, 
       0x3, 0x2, 0x2, 0x2, 0x13c, 0x13b, 0x3, 0x2, 0x2, 0x2, 0x13d, 0x2f, 
       0x3, 0x2, 0x2, 0x2, 0x13e, 0x141, 0x5, 0x48, 0x25, 0x2, 0x13f, 0x141, 
       0x5, 0x46, 0x24, 0x2, 0x140, 0x13e, 0x3, 0x2, 0x2, 0x2, 0x140, 0x13f, 
       0x3, 0x2, 0x2, 0x2, 0x141, 0x31, 0x3, 0x2, 0x2, 0x2, 0x142, 0x14d, 
       0x5, 0x2e, 0x18, 0x2, 0x143, 0x144, 0x7, 0x1f, 0x2, 0x2, 0x144, 0x146, 
       0x7, 0x27, 0x2, 0x2, 0x145, 0x147, 0x5, 0x36, 0x1c, 0x2, 0x146, 0x145, 
       0x3, 0x2, 0x2, 0x2, 0x146, 0x147, 0x3, 0x2, 0x2, 0x2, 0x147, 0x148, 
       0x3, 0x2, 0x2, 0x2, 0x148, 0x14d, 0x7, 0x28, 0x2, 0x2, 0x149, 0x14a, 
       0x5, 0x34, 0x1b, 0x2, 0x14a, 0x14b, 0x5, 0x32, 0x1a, 0x2, 0x14b, 
       0x14d, 0x3, 0x2, 0x2, 0x2, 0x14c, 0x142, 0x3, 0x2, 0x2, 0x2, 0x14c, 
       0x143, 0x3, 0x2, 0x2, 0x2, 0x14c, 0x149, 0x3, 0x2, 0x2, 0x2, 0x14d, 
       0x33, 0x3, 0x2, 0x2, 0x2, 0x14e, 0x14f, 0x9, 0x4, 0x2, 0x2, 0x14f, 
       0x35, 0x3, 0x2, 0x2, 0x2, 0x150, 0x153, 0x5, 0x28, 0x15, 0x2, 0x151, 
       0x153, 0x7, 0x7, 0x2, 0x2, 0x152, 0x150, 0x3, 0x2, 0x2, 0x2, 0x152, 
       0x151, 0x3, 0x2, 0x2, 0x2, 0x153, 0x15b, 0x3, 0x2, 0x2, 0x2, 0x154, 
       0x157, 0x7, 0x25, 0x2, 0x2, 0x155, 0x158, 0x5, 0x28, 0x15, 0x2, 0x156, 
       0x158, 0x7, 0x7, 0x2, 0x2, 0x157, 0x155, 0x3, 0x2, 0x2, 0x2, 0x157, 
       0x156, 0x3, 0x2, 0x2, 0x2, 0x158, 0x15a, 0x3, 0x2, 0x2, 0x2, 0x159, 
       0x154, 0x3, 0x2, 0x2, 0x2, 0x15a, 0x15d, 0x3, 0x2, 0x2, 0x2, 0x15b, 
       0x159, 0x3, 0x2, 0x2, 0x2, 0x15b, 0x15c, 0x3, 0x2, 0x2, 0x2, 0x15c, 
       0x37, 0x3, 0x2, 0x2, 0x2, 0x15d, 0x15b, 0x3, 0x2, 0x2, 0x2, 0x15e, 
       0x15f, 0x8, 0x1d, 0x1, 0x2, 0x15f, 0x160, 0x5, 0x32, 0x1a, 0x2, 0x160, 
       0x166, 0x3, 0x2, 0x2, 0x2, 0x161, 0x162, 0xc, 0x3, 0x2, 0x2, 0x162, 
       0x163, 0x9, 0x5, 0x2, 0x2, 0x163, 0x165, 0x5, 0x32, 0x1a, 0x2, 0x164, 
       0x161, 0x3, 0x2, 0x2, 0x2, 0x165, 0x168, 0x3, 0x2, 0x2, 0x2, 0x166, 
       0x164, 0x3, 0x2, 0x2, 0x2, 0x166, 0x167, 0x3, 0x2, 0x2, 0x2, 0x167, 
       0x39, 0x3, 0x2, 0x2, 0x2, 0x168, 0x166, 0x3, 0x2, 0x2, 0x2, 0x169, 
       0x16a, 0x8, 0x1e, 0x1, 0x2, 0x16a, 0x16b, 0x5, 0x38, 0x1d, 0x2, 0x16b, 
       0x171, 0x3, 0x2, 0x2, 0x2, 0x16c, 0x16d, 0xc, 0x3, 0x2, 0x2, 0x16d, 
       0x16e, 0x9, 0x6, 0x2, 0x2, 0x16e, 0x170, 0x5, 0x38, 0x1d, 0x2, 0x16f, 
       0x16c, 0x3, 0x2, 0x2, 0x2, 0x170, 0x173, 0x3, 0x2, 0x2, 0x2, 0x171, 
       0x16f, 0x3, 0x2, 0x2, 0x2, 0x171, 0x172, 0x3, 0x2, 0x2, 0x2, 0x172, 
       0x3b, 0x3, 0x2, 0x2, 0x2, 0x173, 0x171, 0x3, 0x2, 0x2, 0x2, 0x174, 
       0x175, 0x8, 0x1f, 0x1, 0x2, 0x175, 0x176, 0x5, 0x3a, 0x1e, 0x2, 0x176, 
       0x17c, 0x3, 0x2, 0x2, 0x2, 0x177, 0x178, 0xc, 0x3, 0x2, 0x2, 0x178, 
       0x179, 0x9, 0x7, 0x2, 0x2, 0x179, 0x17b, 0x5, 0x3a, 0x1e, 0x2, 0x17a, 
       0x177, 0x3, 0x2, 0x2, 0x2, 0x17b, 0x17e, 0x3, 0x2, 0x2, 0x2, 0x17c, 
       0x17a, 0x3, 0x2, 0x2, 0x2, 0x17c, 0x17d, 0x3, 0x2, 0x2, 0x2, 0x17d, 
       0x3d, 0x3, 0x2, 0x2, 0x2, 0x17e, 0x17c, 0x3, 0x2, 0x2, 0x2, 0x17f, 
       0x180, 0x8, 0x20, 0x1, 0x2, 0x180, 0x181, 0x5, 0x3c, 0x1f, 0x2, 0x181, 
       0x187, 0x3, 0x2, 0x2, 0x2, 0x182, 0x183, 0xc, 0x3, 0x2, 0x2, 0x183, 
       0x184, 0x9, 0x8, 0x2, 0x2, 0x184, 0x186, 0x5, 0x3c, 0x1f, 0x2, 0x185, 
       0x182, 0x3, 0x2, 0x2, 0x2, 0x186, 0x189, 0x3, 0x2, 0x2, 0x2, 0x187, 
       0x185, 0x3, 0x2, 0x2, 0x2, 0x187, 0x188, 0x3, 0x2, 0x2, 0x2, 0x188, 
       0x3f, 0x3, 0x2, 0x2, 0x2, 0x189, 0x187, 0x3, 0x2, 0x2, 0x2, 0x18a, 
       0x18b, 0x8, 0x21, 0x1, 0x2, 0x18b, 0x18c, 0x5, 0x3e, 0x20, 0x2, 0x18c, 
       0x192, 0x3, 0x2, 0x2, 0x2, 0x18d, 0x18e, 0xc, 0x3, 0x2, 0x2, 0x18e, 
       0x18f, 0x7, 0x1d, 0x2, 0x2, 0x18f, 0x191, 0x5, 0x3e, 0x20, 0x2, 0x190, 
       0x18d, 0x3, 0x2, 0x2, 0x2, 0x191, 0x194, 0x3, 0x2, 0x2, 0x2, 0x192, 
       0x190, 0x3, 0x2, 0x2, 0x2, 0x192, 0x193, 0x3, 0x2, 0x2, 0x2, 0x193, 
       0x41, 0x3, 0x2, 0x2, 0x2, 0x194, 0x192, 0x3, 0x2, 0x2, 0x2, 0x195, 
       0x196, 0x8, 0x22, 0x1, 0x2, 0x196, 0x197, 0x5, 0x40, 0x21, 0x2, 0x197, 
       0x19d, 0x3, 0x2, 0x2, 0x2, 0x198, 0x199, 0xc, 0x3, 0x2, 0x2, 0x199, 
       0x19a, 0x7, 0x1e, 0x2, 0x2, 0x19a, 0x19c, 0x5, 0x40, 0x21, 0x2, 0x19b, 
       0x198, 0x3, 0x2, 0x2, 0x2, 0x19c, 0x19f, 0x3, 0x2, 0x2, 0x2, 0x19d, 
       0x19b, 0x3, 0x2, 0x2, 0x2, 0x19d, 0x19e, 0x3, 0x2, 0x2, 0x2, 0x19e, 
       0x43, 0x3, 0x2, 0x2, 0x2, 0x19f, 0x19d, 0x3, 0x2, 0x2, 0x2, 0x1a0, 
       0x1a1, 0x5, 0x3a, 0x1e, 0x2, 0x1a1, 0x45, 0x3, 0x2, 0x2, 0x2, 0x1a2, 
       0x1a3, 0x9, 0x9, 0x2, 0x2, 0x1a3, 0x47, 0x3, 0x2, 0x2, 0x2, 0x1a4, 
       0x1a5, 0x9, 0xa, 0x2, 0x2, 0x1a5, 0x49, 0x3, 0x2, 0x2, 0x2, 0x2d, 
       0x4d, 0x54, 0x58, 0x61, 0x6f, 0x7c, 0x7f, 0x82, 0x8a, 0x95, 0x9f, 
       0xa9, 0xae, 0xb7, 0xba, 0xbd, 0xc3, 0xcf, 0xdc, 0xdf, 0xe5, 0xec, 
       0xf3, 0xf5, 0xfd, 0x108, 0x10e, 0x123, 0x126, 0x133, 0x13c, 0x140, 
       0x146, 0x14c, 0x152, 0x157, 0x15b, 0x166, 0x171, 0x17c, 0x187, 0x192, 
       0x19d, 
  };

  _serializedATN.insert(_serializedATN.end(), serializedATNSegment0,
    serializedATNSegment0 + sizeof(serializedATNSegment0) / sizeof(serializedATNSegment0[0]));


  atn::ATNDeserializer deserializer;
  _atn = deserializer.deserialize(_serializedATN);

  size_t count = _atn.getNumberOfDecisions();
  _decisionToDFA.reserve(count);
  for (size_t i = 0; i < count; i++) { 
    _decisionToDFA.emplace_back(_atn.getDecisionState(i), i);
  }
}

SysYParser::Initializer SysYParser::_init;
