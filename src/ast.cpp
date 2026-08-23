#include "ast.hpp"

namespace yuzu
{
    void SyntaxTree::free_all()
    {
        for (Node* node : nodes)
        {
            delete node;
        }
    }
}