/*
 * Copyright 2009- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation
 * nor does it submit to any jurisdiction.
 */

#ifndef ecflow_viewer_AstCollateVNodeVisitor_HPP
#define ecflow_viewer_AstCollateVNodeVisitor_HPP

#include <vector>

#include "ecflow/node/ExprAstVisitor.hpp"

class VItem;

class AstCollateVNodesVisitor : public ecf::ExprAstVisitor {
public:
    explicit AstCollateVNodesVisitor(std::vector<VItem*>&);
    ~AstCollateVNodesVisitor() override;

    void visitTop(AstTop*) override {}
    void visitRoot(AstRoot*) override {}
    void visitAnd(AstAnd*) override {}
    void visitNot(AstNot*) override {}
    void visitPlus(AstPlus*) override {}
    void visitMinus(AstMinus*) override {}
    void visitDivide(AstDivide*) override {}
    void visitMultiply(AstMultiply*) override {}
    void visitModulo(AstModulo*) override {}
    void visitOr(AstOr*) override {}
    void visitEqual(AstEqual*) override {}
    void visitNotEqual(AstNotEqual*) override {}
    void visitLessEqual(AstLessEqual*) override {}
    void visitGreaterEqual(AstGreaterEqual*) override {}
    void visitGreaterThan(AstGreaterThan*) override {}
    void visitLessThan(AstLessThan*) override {}
    void visitLeaf(AstLeaf*) override {}
    void visitInteger(AstInteger*) override {}
    void visitFunction(AstFunction*) override {}
    void visitNodeState(AstNodeState*) override {}
    void visitEventState(AstEventState*) override;
    void visitNode(AstNode*) override;
    void visitVariable(AstVariable*) override;
    void visitParentVariable(AstParentVariable*) override;
    void visitFlag(AstFlag*) override;

private:
    std::vector<VItem*>& items_;
};

#endif /* ecflow_viewer_AstCollateVNodeVisitor_HPP */
