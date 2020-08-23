/*
 * H9 project
 *
 * Created by SQ8KFH on 2019-06-29.
 *
 * Copyright (C) 2019 Kamil Palkowski. All rights reserved.
 */

#ifndef H9_EXPRESSION_H
#define H9_EXPRESSION_H

#include "config.h"

#include <iostream>

#include "abstractexp.h"
#include "abstractcommand.h"


class NodeExp: public AbstractExp {
    static const char * completion_list[];
public:
    const int node_id;
    NodeExp(int node_id): node_id(node_id) {
        //std::cout << "NodeExp\n";
    }

    const char** get_completion_list() {
        return completion_list;
    }

    ~NodeExp() {
        //std::cout << "~NodeExp\n";
    }
};

class NodeRegExp: public AbstractExp {
    static const char * completion_list[];
public:
    NodeExp* const node;
    const int reg_number;
    NodeRegExp(NodeExp* node, int reg_number): node(node), reg_number(reg_number) {
        //std::cout << "NodeRegExp\n";
    }

    const char** get_completion_list() {
        return completion_list;
    }

    ~NodeRegExp() {
        //std::cout << "~NodeRegExp\n";
        delete node;
    }
};

class NodeRestart: public AbstractCommand {
public:
    NodeExp* const node;
    NodeRestart(NodeExp* node): node(node) {
        //std::cout << "NodeRestart\n";
    }

    void operator()(CommandCtx* ctx);

    ~NodeRestart() {
        //std::cout << "~NodeRestart\n";
        delete node;
    }
};

class NodeGetReg: public AbstractCommand {
public:
    NodeRegExp* const reg;
    NodeGetReg(NodeRegExp* reg): reg(reg) {
        //std::cout << "NodeGetReg\n";
    }

    void operator()(CommandCtx* ctx);

    ~NodeGetReg() {
        //std::cout << "~NodeGetReg\n";
        delete reg;
    }
};

class NodeSetReg: public AbstractCommand {
public:
    NodeRegExp* const reg;
    const int value;
    NodeSetReg(NodeRegExp* reg, int value): reg(reg), value(value) {
    }

    ~NodeSetReg() {
        delete reg;
    }
    void print_response(CommandCtx* ctx);
};

class NodeSetBit: public NodeSetReg {
public:
    const int bit_num;
    NodeSetBit(NodeRegExp* reg, int bit_num): NodeSetReg(reg, 0), bit_num(bit_num) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeClearBit: public NodeSetReg {
public:
    const int bit_num;
    NodeClearBit(NodeRegExp* reg, int bit_num): NodeSetReg(reg, 0), bit_num(bit_num) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeToggleBit: public NodeSetReg {
public:
    const int bit_num;
    NodeToggleBit(NodeRegExp* reg, int bit_num): NodeSetReg(reg, 0), bit_num(bit_num) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeSet1Reg: public NodeSetReg {
public:
    NodeSet1Reg(NodeRegExp* reg, int value): NodeSetReg(reg, value) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeSet2Reg: public NodeSetReg {
public:
    NodeSet2Reg(NodeRegExp* reg, int value): NodeSetReg(reg, value) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeSet3Reg: public NodeSetReg {
public:
    NodeSet3Reg(NodeRegExp* reg, int value): NodeSetReg(reg, value) {
    }

    void operator()(CommandCtx* ctx);
};

class NodeSet4Reg: public NodeSetReg {
public:
    NodeSet4Reg(NodeRegExp* reg, int value): NodeSetReg(reg, value) {
    }

    void operator()(CommandCtx* ctx);
};

#endif //H9_EXPRESSION_H
