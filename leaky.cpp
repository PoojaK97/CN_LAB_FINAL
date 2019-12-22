#include iostream
using namespace std;

int main(int argc, const char  argv[]) {
    cout  Enter size of buffer ;
    int bufferSize;
    cin  bufferSize;
    cout  Enter output rate ;
    int outputRate;
    cin  outputRate;
    cout  Enter grant rate ;
    int grantRate;
    cin  grantRate;
    int dataSize;
    int bufferFilled = 0;
    int grantFilled = 0;
    cout  Enter simulation time ;
    int stime;
    cinstime;
    for (int i=0; istime; i++) {
        cout  Enter size of data ;
        cin  dataSize;
            if (dataSize = bufferSize - bufferFilled) {
                cout  dataSize   packets added to buffer endl;
                bufferFilled += dataSize;
            }
            else {
                cout  (dataSize - (bufferSize - bufferFilled))   packets dropped, insufficient space available in buffer endl;
                bufferFilled = bufferSize;
            }
        if (bufferFilled != 0) {
            if (bufferFilled  outputRate) {
                cout  outputRate   packets sent to grant buffer  endl;
                bufferFilled -= outputRate;
                grantFilled+= outputRate;
            }
            else {
                cout  bufferFilled   packets sent to grant buffer  endl;
                grantFilled+= bufferFilled;
                bufferFilled = 0;
            }
        }
        else {
            cout  No data to send to grant buffer, main buffer empty  endl;
        }
        for (int j=0; jgrantRate; j++) { One packet in 1g seconds
            if (grantFilled  0) {
                cout1 packet sentendl;
                grantFilled--;
            }
           else break;
        }
        cout  Grant buffer has   grantFilled   packets. endl;
    }
    return 0;
}
