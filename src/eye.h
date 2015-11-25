class Eye
{
  public:
    Eye(FaceBase* fb);
    int run();
  private:
    void showFace(Face* face);
    void realtimeWatch(void);
    void realtimeView(void);
    void recognize(void);
    void cycleAllPhotos(void);
    void viewNewPhotos(void);
    void status(void);
    FaceBase *eye_facebase;
};
