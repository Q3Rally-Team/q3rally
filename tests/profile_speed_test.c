#include "../engine/code/qcommon/q_shared.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "../engine/code/game/g_profile_speed.h"

void QDECL Com_Error(int level, const char *fmt, ...) { (void)level; (void)fmt; assert(0); }
void QDECL Com_Printf(const char *fmt, ...) { (void)fmt; }

int main(void) {
    vec3_t angles = {0, 0, 0}, forward, velocity = {100, 900, 800};
    AngleVectors(angles, forward, NULL, NULL);
    assert(fabs(DotProduct(velocity, forward)) == 100);
    velocity[0] = -100;
    assert(fabs(DotProduct(velocity, forward)) == 100);
    angles[0] = -30;
    AngleVectors(angles, forward, NULL, NULL);
    VectorScale(forward, 100, velocity);
    assert(fabs(fabs(DotProduct(velocity, forward)) - 100) < 0.001);

    profile_speed_window_t w = {0};
    int t;
    double result = 0;
    assert(G_Profile_SustainedSpeed(&w, 248, 100, 1) == 0);
    for (t=150; t<=350; t+=50) result=G_Profile_SustainedSpeed(&w,248,t,1);
    assert(result == 248);
    /* A single 400 km/h reading surrounded by 200 cannot set a record. */
    G_Profile_SustainedSpeed(&w,200,400,1);
    G_Profile_SustainedSpeed(&w,400,450,1);
    for(t=500;t<=600;t+=50) result=G_Profile_SustainedSpeed(&w,200,t,1);
    assert(result==200);
    /* Invalid/dead/spawn frames, implausible values and gaps break the window. */
    assert(G_Profile_SustainedSpeed(&w,248,650,0)==0);
    assert(G_Profile_SustainedSpeed(&w,248,700,1)==0);
    assert(G_Profile_SustainedSpeed(&w,248,1000,1)==0);
    assert(G_Profile_SustainedSpeed(&w,401,1050,1)==0);
    assert(G_Profile_SustainedSpeed(&w,NAN,1100,1)==0);
    assert(G_Profile_SustainedSpeed(&w,INFINITY,1150,1)==0);
    assert(G_Profile_SustainedSpeed(&w,248,1200,1)==0);
    assert(G_Profile_SustainedSpeed(&w,248,1199,1)==0);
    /* Duplicate timestamps do not count toward the confirmation time. */
    for(t=0;t<100;t++) assert(G_Profile_SustainedSpeed(&w,248,1199,1)==0);
    assert(G_Profile_SustainedSpeed(&w,248,1449,1)==248);
    puts("ok");
    return 0;
}
