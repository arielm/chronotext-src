#pragma once

#include "chr/cross/Context.h"
#include "chr/gl/Batch.h"
#include "chr/gl/shaders/ColorShader.h"
#include "chr/gl/shaders/TextureAlphaShader.h"
#include "chr/path/FollowablePath2D.h"
#include "chr/io/BinaryInputStream.h"
#include "chr/io/BinaryOutputStream.h"

#include "common/Tracer.h"
#include "common/tunnel/StringSet.h"
#include "common/stroke/Hairline.h"
#include "common/xf/FontManager.h"

#include "AudioLoop.h"
#include "ButtonBar.h"

struct SoundUnit
{
    float volumeTarget;
    float volumeInterp;
    float freqTarget;
    float phase;
    float phaseAdjust;

    SoundUnit()
    :
    volumeInterp(0),
    phase(0),
    phaseAdjust(0)
    {}
};

struct CustomStringData : public StringData
{
    std::u16string text;

    explicit CustomStringData(std::u16string text)
    :
    text(std::move(text))
    {}
};

class Sketch : public chr::CrossSketch, public AudioLoopDelegate
{
public:
    enum State
    {
        UNDEFINED,
        ROLLING,
        TRACING,
        CANCELLED
    };
    
    Sketch();
    virtual ~Sketch() {}

    void start(StartReason reason) final;
    void stop(StopReason reason) final;
    void accelerated(chr::AccelEvent event) final;

    void setup() final;
    void shutdown() final;
    void resize() final;
    void update() final;
    void draw() final;

    void addTouch(int index, float x, float y) final;
    void updateTouch(int index, float x, float y) final;
    void removeTouch(int index, float x, float y) final;
    
    void startSound();
    void stopSound();

    void updateText(const std::string text);
    NSData* getSceneData();
    void setSceneData(NSData *data);
    void cancelScene();
    void restoreScene();
    void setDefaultScene();

protected:
    chr::xf::FontManager fontManager;
    std::shared_ptr<chr::xf::Font> font;
    chr::xf::FontSequence sequence;
    float spaceWidth;

    chr::gl::shaders::ColorShader colorShader;
    chr::gl::shaders::TextureAlphaShader textureAlphaShader;
    
    chr::gl::TextureManager textureManager;

    chr::gl::Texture dotTexture;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> dotBatch;

    chr::gl::State flatState;
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.RGBA> flatBatch;

    State state;
    
    Tracer tracer;

    StringSetRef set;
    
    chr::Hairline hairline;
    chr::Hairline hairlineTracing;

    float scale;
    float fingerRadius;

    bool touchEnabled;
    int numTouches;
    IdProvider idProvider;

    float ax, ay;
    SimulationParams simulationParams;

    AudioLoop audioLoop;
    bool muted;
    bool fadeInRequest, fadeOutRequest;

    std::vector<SoundUnit> units;
    int activeUnitCount;
    
    float toolBarTop;
        
    chr::gl::IndexedVertexBatch<chr::gl::XYZ.UV> buttonBatch;
    ButtonBar buttonBar;
    ButtonBar buttonBarTracing;

    std::vector<glm::vec2> loadPoints(const chr::InputSource &inputSource);
    std::vector<std::u16string> filterText(const std::u16string &input);
    std::u16string sanitizeText(const std::u16string &input);

    glm::vec2 transformCoordinates(float x, float y) const;

    bool checkWordsFit(const chr::path::FollowablePath2D &path, const std::vector<std::u16string> &words);
    void addStrings(StringSet &set, const std::vector<std::u16string> &words, float offset);
    String* createString(const std::u16string &text, float offset);
    std::vector<std::u16string> extractWords(const StringSet &set);

    void drawPath(const chr::path::FollowablePath2D &path);
    void drawSet(const StringSet &set);
    void drawString(const chr::path::FollowablePath2D &path, String *string);

    void initSound();
    void shutdownSound();
    void updateSound();
    void createUnits();
    void populateUnit(SoundUnit &unit, const chr::path::FollowablePath2D &path, String *string);
    float offsetToFreq(const chr::path::FollowablePath2D &path, float position, float minFreq, float maxFreq);
    static float charToFreq(char16_t c);
    void renderCallback(int bufferSize, float *bufferData, double timeStamp) override;
    
    void beginTrace();
    void endTrace();
    void updateTrace(const glm::vec2 &point);
    
    void readSet(chr::io::BinaryInputStream &stream);
    void writeSet(chr::io::BinaryOutputStream &stream);
    void persistScene();

    float getDefaultScale() const;
    void adaptScale();
};
