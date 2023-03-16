#ifndef PUMP_CONTROL_H_
#define PUMP_CONTROL_H_

namespace _10klab
{
    namespace pumps
    {
        void PumpsInitialization();
        void PriorityOrder( int pump1, int ml1, int priority1, bool rotation1, float ka1, float kb1,
                            int pump2, int ml2, int priority2, bool rotation2, float ka2, float kb2,
                            int pump3, int ml3, int priority3, bool rotation3, float ka3, float kb3,
                            int pump4, int ml4, int priority4, bool rotation4, float ka4, float kb4,
                            int pump5, int ml5, int priority5, bool rotation5, float ka5, float kb5,
                            int pump6, int ml6, int priority6, bool rotation6, float ka6, float kb6                                                
                            );
    }
}

#endif