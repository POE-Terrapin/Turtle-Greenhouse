#ifndef __UTILS_H__
#define __UTILS_H__

template<typename T>
void serialWrite(T val){
  Serial.println(val);
  Serial.print('\n'); 
}

template<typename H, typename... T>
void serialWrite(H head, T... tail){
  Serial.print(head);
  Serial.print(',');
  serialWrite(tail...);  
}

// Constant values and pins.
enum {LEFT_MOVE, LEFT_UP, RIGHT_MOVE, RIGHT_UP};
enum {STOP, GO_FORWARD, TURN_LEFT, TURN_RIGHT};


#endif

