#include "Sketch.h"
#include "PathHelper.h"

#include "chr/gl/draw/Sprite.h"
#include "chr/gl/draw/Rect.h"

#include "common/cinder/CinderMath.h"

#import "Bridge.h"

using namespace std;
using namespace chr;
using namespace gl;
using namespace path;
using namespace io;

static constexpr float TRACER_CHUNK_LENGTH = 48;
static constexpr float TRACER_TOL = 3.0;
static constexpr float MAX_TRACED_PATH_LENGTH = 15000;

static constexpr float FINGER_RADIUS_DP = 43;
static constexpr float PATH_OFFSET = 0;
static constexpr float FONT_SIZE = 32;

static constexpr float DT = 1.0f;
static constexpr float FRICTION = 0.0175f;
static constexpr float GRAVITY = 1.0f;

static constexpr float BUTTON_BAR_H = 80;

#define VOLUME_MAX_VELOCITY 50
#define VOLUME_INTERPOLATION_FACTOR 0.0125f
#define VOLUME_ATTENUATION_FACTOR 0.2f

#define SOUND_MIN_VOLUME 0
#define SOUND_MAX_VOLUME 1

#define SOUND_UNITS_CAPACITY 36

Sketch::Sketch()
:
state(State::UNDEFINED),
touchEnabled(true),
numTouches(0),
idProvider(2),
muted(false),
fadeInRequest(false),
fadeOutRequest(false)
{}

void Sketch::start(StartReason reason)
{
    ax = 0;
    ay = 0;

    delegate().enableAccelerometer(15);

    startSound();
}

void Sketch::stop(StopReason reason)
{
    delegate().disableAccelerometer();

    stopSound();
}

void Sketch::accelerated(AccelEvent event)
{
    ax = +event.getRawData().x;
    ay = -event.getRawData().y;
}

void Sketch::setup()
{
    hairline = Hairline(textureManager);

    hairline.strip
        .setShader(textureAlphaShader)
        .setShaderColor(1, 0, 0, 0.67f);

    hairlineTracing = Hairline(textureManager);

    hairlineTracing.strip
        .setShader(textureAlphaShader)
        .setShaderColor(1, 0, 0, 0.67f);

    dotTexture = Texture(
        Texture::ImageRequest("dot2x.png")
            .setFlags(image::FLAGS_TRANSLUCENT)
            .setMipmap(true));
    
    dotBatch
        .setTexture(dotTexture)
        .setShader(textureAlphaShader)
        .setShaderColor(1, 0, 0, 0.67f);

    // ---
    
    font = fontManager.getFont(InputSource::resource("Georgia_Regular_64.fnt"), XFont::Properties2d());
    font->setShader(textureAlphaShader);
    font->setSize(FONT_SIZE);
    spaceWidth = font->getCharAdvance(u' ');

    // ---

    simulationParams.dt = DT;
    simulationParams.friction = FRICTION;
    simulationParams.gravity = GRAVITY;

    fingerRadius = FINGER_RADIUS_DP * getDisplayInfo().density / DisplayInfo::REFERENCE_DENSITY;

    tracer = Tracer(TRACER_CHUNK_LENGTH, TRACER_TOL);

    // ---
    
    initSound();

    // ---
    
    [[Bridge sharedInstance] setSketch:this];

    // ---

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Sketch::shutdown()
{
    shutdownSound();
}

/*
 * We rely on the fact that Sketch::resize() will be called only once
 */
void Sketch::resize()
{
    scale = getDefaultScale();
    float contentScale = getDisplayInfo().contentScale;
    
    toolBarTop = windowInfo.height - windowInfo.safeAreaInsetsBottom - BUTTON_BAR_H * contentScale;
    
    flatBatch.setShader(colorShader);
    
    draw::Rect()
      .setColor(0, 0, 0, 1)
      .setBounds(0, toolBarTop, windowInfo.width, windowInfo.height - toolBarTop)
      .append(flatBatch);

    buttonBatch
        .setShader(textureAlphaShader)
        .setShaderColor(1, 1, 1, 1);
    
    ButtonBar::Properties properties;
    properties.windowWidth = windowInfo.width;
    properties.height = BUTTON_BAR_H;
    properties.toolBarTop = toolBarTop;
    properties.contentScale = contentScale;
    
    vector<string> names = {"icon_draw.png", "icon_type.png", "icon_export.png", "icon_import.png", "icon_info.png"};
    buttonBar.init(properties, names);
    
    vector<string> namesTracing = {"icon_back.png"};
    buttonBarTracing.init(properties, namesTracing);
    
    if (state != State::CANCELLED)
    {
        setDefaultScene();
    }
}

void Sketch::update()
{
    if (state == State::ROLLING)
    {
        set->update(ax, ay, simulationParams);
        updateSound();
    }
}

void Sketch::draw()
{
    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    auto projectionMatrix = glm::ortho(0.0f, windowInfo.width, windowInfo.height, 0.0f);

    // ---

    Matrix modelViewMatrix;
    modelViewMatrix
        .translate(windowInfo.width * 0.5f, windowInfo.safeAreaInsetsTop + (toolBarTop - windowInfo.safeAreaInsetsTop) * 0.5f)
        .scale(scale);

    chr::gl::State()
        .setShaderMatrix(modelViewMatrix * projectionMatrix)
        .apply();

    if (state == State::TRACING)
    {
        hairlineTracing.strip.flush();
    }
    else if (state == State::ROLLING)
    {
        hairline.strip.flush();

        font->beginSequence(sequence);

        drawSet(*set);

        font->endSequence();
        font->replaySequence(sequence);
    }
    
    // ---
    
    chr::gl::State()
        .setShaderMatrix(projectionMatrix)
        .apply();
    
    flatBatch.flush();
    
    if (state == State::TRACING)
    {
        buttonBarTracing.draw(buttonBatch);
    }
    else if (state == State::ROLLING)
    {
        buttonBar.draw(buttonBatch);
    }
}

void Sketch::addTouch(int index, float x, float y)
{
    updateTouch(index, x, y);
    numTouches++;
    
    if (state == State::ROLLING)
    {
        int buttonIndex = buttonBar.checkTouch(x, y);
        if (buttonIndex > -1)
        {
            switch (buttonIndex)
            {
                case 0:
                    beginTrace();
                    break;
                    
                case 1:
                    [[Bridge sharedInstance] showTextInput];
                    break;
                    
                case 2:
                    [[Bridge sharedInstance] showExportScreen];
                    break;
                    
                case 3:
                    [[Bridge sharedInstance] showImportScreen];
                    break;

                case 4:
                    [[Bridge sharedInstance] showInfoScreen];
                    break;
            }
        }
    }
    else if (state == State::TRACING)
    {
        int buttonIndex = buttonBarTracing.checkTouch(x, y);
        if (buttonIndex == 0)
        {
            state = State::ROLLING;
            fadeInRequest = true;
        }
    }
}

void Sketch::updateTouch(int index, float x, float y)
{
    if (state == State::TRACING)
    {
        if (index == 0)
        {
            updateTrace(transformCoordinates(x, y));
        }
    }
    else if (state == State::ROLLING)
    {
        if (touchEnabled)
        {
            double timestamp = getElapsedSeconds();
            set->updateTouch(index, transformCoordinates(x, y), timestamp);
        }
    }
}

void Sketch::removeTouch(int index, float x, float y)
{
    if (state == State::TRACING)
    {
        if (index == 0)
        {
            endTrace();
        }
    }
    else if (state == State::ROLLING)
    {
        if (touchEnabled)
        {
            set->removeTouch(index, transformCoordinates(x, y));
        }
    }

    if (--numTouches == 0)
    {
        idProvider.reset(0);
    }
}

void Sketch::updateText(const std::string text)
{
    if (text.size() > 0)
    {
        set->removeStrings();
        
        auto words = filterText(utils::to<u16string>(text));
        if (checkWordsFit(set->path, words))
        {
            addStrings(*set, words, 0);
            persistScene();
        }
        else
        {
            [[Bridge sharedInstance] showTextTooLongAlert];
        }
    }
    
    state = State::ROLLING;
}

glm::vec2 Sketch::transformCoordinates(float x, float y) const
{
    return (glm::vec2(x, y) - glm::vec2(windowInfo.width * 0.5f, windowInfo.safeAreaInsetsTop + (toolBarTop - windowInfo.safeAreaInsetsTop) * 0.5f)) / scale;
}

vector<glm::vec2> Sketch::loadPoints(const InputSource &inputSource)
{
    vector<glm::vec2> points;
    BinaryInputStream stream(inputSource);

    if (stream.good())
    {
        auto count = stream.read<uint32_t>();
        points.reserve(count);

        for (auto i = 0; i < count; i++)
        {
            points.emplace_back(stream.read<glm::vec2>());
        }
    }

    return points;
};

/*
 * SPLIT STRING TO SANITIZED WORDS
 */
vector<u16string> Sketch::filterText(const u16string &input)
{
    vector<u16string> output;
    u16string buffer;
    auto sanitized = sanitizeText(input);

    for (auto c : sanitized)
    {
        if (font->isSpace(c))
        {
            if (!buffer.empty())
            {
                output.push_back(buffer);
            }

            buffer.clear();
        }
        else
        {
            buffer.append(1, c);
        }
    }

    if (!buffer.empty())
    {
        output.push_back(buffer);
    }

    return output;
}

/*
 * - TURNS RETURNS INTO SPACES
 * - TRIMS SPACES AT START AND END
 * - LIMITS TO ONE CONSECUTIVE SPACES
 * - TURNS INVALID CHARACTERS TO QUESTION MARKS
 */
u16string Sketch::sanitizeText(const u16string &input)
{
    u16string input2;
    for (auto c : input)
    {
        if (c == u'\n')
        {
            input2.append(1, u' ');
        }
        else
        {
            input2.append(1, c);
        }
    }
    
    u16string output;
    int spaceCount = 0;
    bool beginnings = true;

    for (auto c : input2)
    {
        if (font->isValid(c))
        {
            if (font->isSpace(c) || c == u'\n')
            {
                spaceCount++;
            }
            else
            {
                spaceCount = 0;
                beginnings = false;
            }

            if (!beginnings && (spaceCount < 2))
            {
                output.append(1, c);
            }
        }
        else
        {
            output.append(1, u'?');
            spaceCount = 0;
            beginnings = false;
        }
    }

    if (spaceCount > 0)
    {
        return output.substr(0, output.size() - 1);
    }
    else
    {
        return output;
    }
}

bool Sketch::checkWordsFit(const FollowablePath2D &path, const vector<u16string> &words)
{
    float length = spaceWidth;

    for (auto &word : words)
    {
        length += font->getStringAdvance(word) + spaceWidth;
    }

    return path.getLength() >= length;
}

void Sketch::addStrings(StringSet &set, const vector<u16string> &words, float offset)
{
    for (auto &word : words)
    {
        set.addString(createString(word, offset));
        offset += set.spaceWidth + font->getStringAdvance(word);
    }
}

String* Sketch::createString(const u16string &text, float offset)
{
    int size = text.size();
    String *string = new String(size);

    float length = 0;
    offset -= PATH_OFFSET;

    for (int i = 0; i < size; i++)
    {
        auto c = text[i];
        auto w = font->getCharAdvance(c);

        string->setPosition(i, offset + length, w, true);
        length += w;
    }

    string->length = length;
    string->size = size;

    string->data = new CustomStringData(text);

    return string;
}

vector<u16string> Sketch::extractWords(const StringSet &set)
{
    vector<u16string> words;
    
    for (auto string : set.strings)
    {
        CustomStringData *stringData = (CustomStringData*)string->data;
        words.push_back(stringData->text);
    }
    
    return words;
}

void Sketch::drawPath(const FollowablePath2D &path)
{
    dotBatch.clear();

    Matrix matrix;
    matrix
        .translate(path.offsetToPosition(0))
        .scale(0.5f);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(dotBatch, matrix);

    matrix
        .setTranslate(path.offsetToPosition(path.getLength()))
        .scale(0.5f);

    draw::Sprite()
        .setAnchor(0.5f, 0.5f)
        .append(dotBatch, matrix);

    dotBatch.flush();
}

void Sketch::drawSet(const StringSet &set)
{
    drawPath(set.path);

    for (auto string : set.strings)
    {
        drawString(set.path, string);
    }
}

void Sketch::drawString(const FollowablePath2D &path, String *string)
{
    CustomStringData *stringData = (CustomStringData*)string->data;
    auto &text = stringData->text;

    float offsetX = string->position[0];
    float offsetY = font->getOffsetY(XFont::ALIGN_MIDDLE);
    Matrix matrix;

    for (auto c : text)
    {
        auto glyphIndex = font->getGlyphIndex(c);
        auto half = 0.5f * font->getGlyphAdvance(glyphIndex);
        offsetX += half;

        if (glyphIndex >= 0)
        {
            path
                .offsetToValue(offsetX, FONT_SIZE) // XXX
                .applyToMatrix(matrix);
            font->addGlyph(matrix, glyphIndex, -half, offsetY);
        }

        offsetX += half;
    }
}

void Sketch::initSound()
{
    createUnits();
    activeUnitCount = 0;

    audioLoop.delegate = this;
    audioLoop.init();
}

void Sketch::startSound()
{
    audioLoop.start();
    fadeInRequest = true;
}

void Sketch::stopSound()
{
    fadeOutRequest = true;
}

void Sketch::shutdownSound()
{
    audioLoop.shutdown();
}

void Sketch::updateSound()
{
    muted = false;
    
    int index = 0;

    for (auto &string : set->strings)
    {
        if (index == SOUND_UNITS_CAPACITY)
        {
            break;
        }

        populateUnit(units[index++], set->path, string);
    }

    activeUnitCount = index;
}

void Sketch::createUnits()
{
    for (int i = 0; i < SOUND_UNITS_CAPACITY; i++)
    {
        units.push_back(SoundUnit());
    }
}

void Sketch::populateUnit(SoundUnit &unit, const FollowablePath2D &path, String *string)
{
    CustomStringData *stringData = (CustomStringData*)string->data;
    auto &text = stringData->text;

    float averageFreq = 0;
    float averageVelocity = 0;

    for (int i = 0; i < string->size; i++)
    {
        float minFreq = charToFreq(text[i]);
        float maxFreq = (minFreq == 0) ? 0 : (minFreq + 3520);

        averageFreq += offsetToFreq(path, string->position[i], minFreq, maxFreq);
        averageVelocity += string->position[i] - string->prevPosition[i];
    }

    unit.freqTarget = averageFreq / string->size;
    averageVelocity /= string->size;

    if (string->locked)
    {
        unit.volumeTarget = 0.25f; // XXX
    }
    else
    {
        float u = cinder::math<float>::clamp(fabsf(averageVelocity), 0, VOLUME_MAX_VELOCITY) / VOLUME_MAX_VELOCITY;
        unit.volumeTarget = SOUND_MIN_VOLUME + (SOUND_MAX_VOLUME - SOUND_MIN_VOLUME) * u;
    }
}

float Sketch::offsetToFreq(const FollowablePath2D &path, float offset, float minFreq, float maxFreq)
{
    float freqSampleRadius = FONT_SIZE * scale;
    
    glm::vec2 pm = path.offsetToPosition(offset - freqSampleRadius);
    glm::vec2 pp = path.offsetToPosition(offset + freqSampleRadius);
    glm::vec2 gradient = (pp - pm) * 0.5f;
    float d = glm::length(gradient);

    float u = 1 - cinder::math<float>::clamp(d, 0, freqSampleRadius) / freqSampleRadius;
    return (minFreq + (maxFreq - minFreq) * u);
}

float Sketch::charToFreq(char16_t c)
{
    if (c >= u'A' && c <= u'Z')
    {
        return (c - u'A') * 55 + 55;
    }

    if (c >= u'a' && c <= u'z')
    {
        return (c - u'a') * 55 + 55;
    }

    if (c >= u'0' && c <= u'9')
    {
        return (c - u'0') * 55 + 55 + 55 * 26;
    }
    
    if (c == u'À' || c == u'Á' || c == u'Â' || c == u'Ã' || c == u'Ä' || c == u'Å' || c == u'à' || c == u'á' || c == u'â' || c == u'ã' || c == u'ä' || c == u'å')
    {
        return charToFreq(u'A');
    }
    
    if (c == u'Ç' || c == u'ç')
    {
        return charToFreq(u'C');
    }
    
    if (c == u'È' || c == u'É' || c == u'Ê' || c == u'Ë' || c == u'è' || c == u'é' || c == u'ê' || c == u'ë')
    {
        return charToFreq(u'E');
    }
    
    if (c == u'Ì' || c == u'Í' || c == u'Î' || c == u'Ï' || c == u'ì' || c == u'í' || c == u'î' || c == u'ï')
    {
        return charToFreq(u'I');
    }
    
    if (c == u'Ñ' || c == u'ñ')
    {
        return charToFreq(u'N');
    }
    
    if (c == u'Ò' || c == u'Ó' || c == u'Ô' || c == u'Õ' || c == u'Ö' || c == u'Ø' || c == u'ò' || c == u'ó' || c == u'ô' || c == u'õ' || c == u'ö' || c == u'ø')
    {
        return charToFreq(u'O');
    }
    
    if (c == u'Ù' || c == u'Ú' || c == u'Û' || c == u'Ü' || c == u'ù' || c == u'ú' || c == u'û' || c == u'ü')
    {
        return charToFreq(u'U');
    }
    
    if (c == u'Š' || c == u'š' || c == u'ß')
    {
        return charToFreq(u'S');
    }
    
    if (c == u'Ÿ' || c == u'Ý' || c == u'ý' || c == u'ÿ')
    {
        return charToFreq(u'Y');
    }
    
    if (c == u'Ž' || c == u'ž')
    {
        return charToFreq(u'Z');
    }
    
    return 0;
}

void Sketch::renderCallback(int bufferSize, float *bufferData, double timeStamp)
{
    if (muted)
    {
        for (int  i = 0; i < bufferSize; i++)
        {
            *bufferData++= 0;
        }
        
        return;
    }
    
    bool fadeIn = false, fadeOut = false;
    
    if (fadeInRequest)
    {
        fadeInRequest = false;
        fadeIn = true;
    }
    else if (fadeOutRequest)
    {
        fadeOutRequest = false;
        fadeOut = true;
        muted = true;
    }

    for (int i = 0; i < activeUnitCount; i++)
    {
        SoundUnit &unit = units[i];
        unit.phaseAdjust = unit.phaseAdjust * 0.95f + (unit.freqTarget / 44100) * 0.05f;
        unit.volumeInterp += (unit.volumeTarget - unit.volumeInterp) * VOLUME_INTERPOLATION_FACTOR;
    }

    for (int i = 0; i < bufferSize; i++)
    {
        float value = 0;

        for (int j = 0; j < activeUnitCount; j++)
        {
            SoundUnit &unit = units[j];
            unit.phase += unit.phaseAdjust;
            unit.phase = unit.phase - floorf(unit.phase);
            value += sinf(unit.phase * TWO_PI) * unit.volumeInterp;
        }

        if (activeUnitCount > 0)
        {
            value /= activeUnitCount;
        }
        
        float fadeFactor = 1;
        
        if (fadeIn)
        {
            fadeFactor = i / (float)bufferSize;
        }
        else if (fadeOut)
        {
            fadeFactor = 1 - i / (float)bufferSize;
        }

        *bufferData++= value * VOLUME_ATTENUATION_FACTOR * fadeFactor;
    }
}

void Sketch::beginTrace()
{
    scale = getDefaultScale();
    tracer.begin();
    hairlineTracing.clear();
    
    state = State::TRACING;
    fadeOutRequest = true;
}

void Sketch::endTrace()
{
    tracer.end();

    if (!tracer.path.empty())
    {
        auto words = extractWords(*set);
        if (checkWordsFit(tracer.path, words))
        {
            set = make_shared<StringSet>(tracer.path, PATH_OFFSET, spaceWidth, fingerRadius, idProvider);
            addStrings(*set, words, 0);
            persistScene();
            hairline.stroke(set->path, scale);
            
            state = State::ROLLING;
            fadeInRequest = true;
        }
        else
        {
            [[Bridge sharedInstance] showPathTooShortAlert];
            tracer.begin();
        }
    }
    else
    {
        /*
         * THE FINGER WAS RELEASED BEFORE
         * THAT rawTrace BECAME LONG ENOUGH:
         * LET'S KEEP TRACING...
         */
        tracer.begin();
    }
}

void Sketch::updateTrace(const glm::vec2 &point)
{
    tracer.update(point);

    if (!tracer.path.empty())
    {
        hairlineTracing.stroke(tracer.path, scale);
        
        if (tracer.path.getLength() > MAX_TRACED_PATH_LENGTH)
        {
            endTrace();
        }
    }
}

void Sketch::setSceneData(NSData *data)
{
    auto inputSource = InputSource::buffer([data bytes], data.length);
    auto stream = BinaryInputStream(inputSource);
    readSet(stream);
    
    persistScene();
    
    adaptScale();

    hairline.stroke(set->path, scale);
    
    state = State::ROLLING;
}

void Sketch::readSet(BinaryInputStream &stream)
{
    FollowablePath2D path;
    PathHelper::read(path, stream);
    
    set = make_shared<StringSet>(path, PATH_OFFSET, spaceWidth, fingerRadius, idProvider);
    
    auto count = stream.read<uint32_t>();
    for (auto i = 0; i < count; i++)
    {
        float offset = stream.read<float>();
        u16string word = utils::to<u16string>(stream.readString());
        set->addString(createString(word, offset));
    }
}

NSData* Sketch::getSceneData()
{
    auto outputTarget = OutputTarget::buffer();
    auto stream = BinaryOutputStream(outputTarget);
    writeSet(stream);
    stream.close();

    return [NSData dataWithBytes:outputTarget.getData() length:outputTarget.getDataSize()];
}

void Sketch::writeSet(BinaryOutputStream &stream)
{
    PathHelper::write(set->path, stream);

    stream.write(uint32_t(set->strings.size()));
    
    for (auto string : set->strings)
    {
        stream.write(string->position[0]);

        CustomStringData *stringData = (CustomStringData*)string->data;
        stream.writeString(utils::to<std::string>(stringData->text));
    }
}

void Sketch::cancelScene()
{
    state = State::CANCELLED;
}

void Sketch::restoreScene()
{
    if (set)
    {
        state = State::ROLLING;
    }
    else
    {
        setDefaultScene();
    }
}

void Sketch::setDefaultScene()
{
    if (fs::exists(getDocumentsFolder() / "record.dat"))
    {
        auto stream = BinaryInputStream(InputSource::file(getDocumentsFolder() / "record.dat"));
        readSet(stream);
    }
    else
    {
        auto stream = BinaryInputStream(InputSource::resource("440hz_monster.dat"));
        readSet(stream);
    }

    adaptScale();

    hairline.stroke(set->path, scale);

    state = State::ROLLING;
}

void Sketch::persistScene()
{
    auto outputTarget = OutputTarget::file(getDocumentsFolder() / "record.dat");
    BinaryOutputStream stream(outputTarget);
    writeSet(stream);
}

float Sketch::getDefaultScale() const
{
    return 0.5f * getDisplayInfo().contentScale;
}

void Sketch::adaptScale()
{
    scale = getDefaultScale();
    glm::vec2 dimensions = PathHelper::getDimensions(set->path) * scale;
    float availableWidth = windowInfo.width;
    float availableHeight = toolBarTop - windowInfo.safeAreaInsetsTop;
    
    if (dimensions.x > availableWidth || dimensions.y > availableHeight)
    {
        float ratioX = availableWidth / dimensions.x;
        float ratioY = availableHeight / dimensions.y;
        
        if (ratioX < ratioY)
        {
            scale *= ratioX * 0.9f;
        }
        else
        {
            scale *= ratioY * 0.9f;
        }
    }
}
