# commom
vpath %.cpp antlr4/src
ANTLR_COMM = WritableToken.o Vocabulary.o UnbufferedTokenStream.o UnbufferedCharStream.o \
 			TokenStreamRewriter.o TokenStream.o TokenSource.o RuntimeMetaData.o RuleContextWithAltNum.o \
			Recognizer.o RuleContext.o Token.o RecognitionException.o ProxyErrorListener.o ParserRuleContext.o \
			ParserInterpreter.o Parser.o NoViableAltException.o ListTokenSource.o LexerNoViableAltException.o \
			LexerInterpreter.o Lexer.o IntStream.o InterpreterRuleContext.o FailedPredicateException.o  \
			InputMismatchException.o Exceptions.o DiagnosticErrorListener.o DefaultErrorStrategy.o \
			ConsoleErrorListener.o CommonTokenStream.o CommonTokenFactory.o CommonToken.o CharStream.o \
			BufferedTokenStream.o BaseErrorListener.o BailErrorStrategy.o ANTLRInputStream.o ANTLRFileStream.o \
			ANTLRErrorStrategy.o ANTLRErrorListener.o 

# ant
vpath %.cpp antlr4/src/atn
ANTLR_ANT = AbstractPredicateTransition.o ActionTransition.o AmbiguityInfo.o ArrayPredictionContext.o ATN.o \
			ATNConfig.o ATNConfigSet.o ATNDeserializationOptions.o ATNDeserializer.o ATNSerializer.o ATNSimulator.o \
			ATNState.o AtomTransition.o BasicBlockStartState.o BasicState.o BlockEndState.o BlockStartState.o \
			ContextSensitivityInfo.o DecisionEventInfo.o DecisionInfo.o DecisionState.o EmptyPredictionContext.o \
			EpsilonTransition.o ErrorInfo.o LexerAction.o LexerActionExecutor.o LexerATNConfig.o LexerATNSimulator.o \
			LexerChannelAction.o LexerCustomAction.o LexerIndexedCustomAction.o LexerModeAction.o LexerMoreAction.o \
			LexerPopModeAction.o LexerPushModeAction.o LexerTypeAction.o LexerSkipAction.o LL1Analyzer.o \
			LookaheadEventInfo.o LoopEndState.o NotSetTransition.o OrderedATNConfigSet.o ParseInfo.o ParserATNSimulator.o \
			PlusBlockStartState.o PlusLoopbackState.o PrecedencePredicateTransition.o PredicateEvalInfo.o PredicateTransition.o \
			PredictionContext.o PredictionMode.o ProfilingATNSimulator.o RangeTransition.o RuleStartState.o RuleStopState.o \
			RuleTransition.o SemanticContext.o SetTransition.o SingletonPredictionContext.o StarBlockStartState.o \
			StarLoopbackState.o StarLoopEntryState.o TokensStartState.o Transition.o WildcardTransition.o

# dfa
vpath %.cpp antlr4/src/dfa
ANTLR_DFA = DFA.o DFASerializer.o DFAState.o LexerDFASerializer.o

# misc
vpath %.cpp antlr4/src/misc
ANTLR_MISC = InterpreterDataReader.o Interval.o IntervalSet.o MurmurHash.o Predicate.o

# support
vpath %.cpp antlr4/src/support
ANTLR_SUPPORT = Any.o Arrays.o CPPUtils.o StringUtils.o guid.o

# tree
vpath %.cpp antlr4/src/tree:antlr4/src/tree/pattern:antlr4/src/tree/xpath
ANTLR_TREE = Chunk.o ParseTreeMatch.o ParseTreePattern.o ParseTreePatternMatcher.o RuleTagToken.o TextChunk.o \
			TagChunk.o TokenTagToken.o XPath.o XPathElement.o XPathLexer.o XPathLexerErrorListener.o \
			XPathRuleAnywhereElement.o XPathRuleElement.o XPathTokenAnywhereElement.o XPathTokenElement.o \
			XPathWildcardAnywhereElement.o XPathWildcardElement.o ErrorNode.o ErrorNodeImpl.o IterativeParseTreeWalker.o \
			ParseTree.o ParseTreeListener.o ParseTreeVisitor.o ParseTreeWalker.o TerminalNode.o TerminalNodeImpl.o Trees.o

# all
ANTLR_OBJ = $(ANTLR_COMM) $(ANTLR_ANT) $(ANTLR_DFA) $(ANTLR_MISC) $(ANTLR_SUPPORT) $(ANTLR_TREE)