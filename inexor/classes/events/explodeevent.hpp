#pragma once

/// 
struct explodeevent : timedevent
{
    int id, gun;
    vector<hitinfo> hits;

    bool keepable() const { return true; }

    void process(clientinfo *ci);
};