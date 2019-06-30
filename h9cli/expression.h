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
#include "abstractexp.h"
#include "abstractcommand.h"
#include <iostream>


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

    void operator()() {
        std::cout << "restart\n";
    }

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

    void operator()() {
        std::cout << "get reg\n";
    }

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
        //std::cout << "NodeSetReg\n";
    }

    void operator()() {
        std::cout << "set reg\n";
    }

    ~NodeSetReg() {
        //std::cout << "~NodeSetReg\n";
        delete reg;
    }
};

#endif //H9_EXPRESSION_H
