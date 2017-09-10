#pragma once
#include "Level.h"

class Node
{
public:

private:
    float m_H;      // Heuristic
    float m_G;      // Movement cost
    float m_F;      // G + H
    Node* mp_parent = nullptr;
};


class PathFinder
{
public:
    PathFinder( const Level& lvl );
    ~PathFinder();

    void init( const Level& lvl );

private:
    /* dimensions of the current level/map in tiles */
    int m_width;
    int m_height;

    /* for each cell all h values for the other cells will be precomputed and stored here */
    int* mp_all_H_values = nullptr;

    /* pointer to the place in mp_all_H_values where the h values for the current target are stored */
    int* mp_current_H_values = nullptr;   
};