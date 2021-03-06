#include <mbgl/test/util.hpp>
#include <mbgl/test/stub_file_source.hpp>

#include <mbgl/annotation/annotation.hpp>
#include <mbgl/sprite/sprite_image.hpp>
#include <mbgl/map/map.hpp>
#include <mbgl/platform/default/headless_display.hpp>
#include <mbgl/platform/default/headless_view.hpp>
#include <mbgl/util/io.hpp>
#include <mbgl/util/run_loop.hpp>

#include <future>
#include <vector>

using namespace mbgl;

std::shared_ptr<SpriteImage> namedMarker(const std::string &name) {
    PremultipliedImage image = decodeImage(util::read_file("test/fixtures/sprites/" + name));
    return std::make_shared<SpriteImage>(std::move(image), 1.0);
}

namespace {

void checkRendering(Map& map, const char * name) {
    test::checkImage(std::string("test/fixtures/annotations/") + name,
                     test::render(map), 0.0002, 0.1);
}

} // end namespace

TEST(Annotations, SymbolAnnotation) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    map.addAnnotation(SymbolAnnotation { Point<double>(0, 0), "default_marker" });

    checkRendering(map, "point_annotation");
}

TEST(Annotations, LineAnnotation) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    LineString<double> line = {{ { 0, 0 }, { 45, 45 } }};

    LineAnnotation annotation { line };
    annotation.color = {{ 255, 0, 0, 1 }};
    annotation.width = 5;

    map.addAnnotation(annotation);

    checkRendering(map, "line_annotation");
}

TEST(Annotations, FillAnnotation) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillAnnotation annotation { polygon };
    annotation.color = {{ 255, 0, 0, 1 }};

    map.addAnnotation(annotation);

    checkRendering(map, "fill_annotation");
}

TEST(Annotations, StyleSourcedShapeAnnotation) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/annotation.json"), "");

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    map.addAnnotation(StyleSourcedAnnotation { polygon, "annotation" });

    checkRendering(map, "style_sourced_shape_annotation");
}

TEST(Annotations, AddMultiple) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    map.addAnnotation(SymbolAnnotation { Point<double> { -10, 0 }, "default_marker" });

    test::render(map);

    map.addAnnotation(SymbolAnnotation { Point<double> { 10, 0 }, "default_marker" });

    checkRendering(map, "add_multiple");
}

TEST(Annotations, NonImmediateAdd) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    test::render(map);

    Polygon<double> polygon = {{ {{ { 0, 0 }, { 0, 45 }, { 45, 45 }, { 45, 0 } }} }};

    FillAnnotation annotation { polygon };
    annotation.color = {{ 255, 0, 0, 1 }};

    map.addAnnotation(annotation);

    checkRendering(map, "non_immediate_add");
}

TEST(Annotations, UpdateIcon) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("flipped_marker", namedMarker("default_marker.png"));
    map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "flipped_marker" });

    test::render(map);

    map.removeAnnotationIcon("flipped_marker");
    map.addAnnotationIcon("flipped_marker", namedMarker("flipped_marker.png"));
    map.update(Update::Annotations);

    checkRendering(map, "update_icon");
}

TEST(Annotations, UpdatePoint) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    map.addAnnotationIcon("flipped_marker", namedMarker("flipped_marker.png"));
    AnnotationID point = map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(map);

    map.updateAnnotation(point, SymbolAnnotation { Point<double> { -10, 0 }, "flipped_marker" });

    checkRendering(map, "update_point");
}

TEST(Annotations, RemovePoint) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    AnnotationID point = map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(map);

    map.removeAnnotation(point);

    checkRendering(map, "remove_point");
}

TEST(Annotations, RemoveShape) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    LineString<double> line = {{ { 0, 0 }, { 45, 45 } }};

    LineAnnotation annotation { line };
    annotation.color = {{ 255, 0, 0, 1 }};
    annotation.width = 5;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    AnnotationID shape = map.addAnnotation(annotation);

    test::render(map);

    map.removeAnnotation(shape);

    checkRendering(map, "remove_shape");
}

TEST(Annotations, ImmediateRemoveShape) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;
    Map map(view, fileSource, MapMode::Still);

    map.removeAnnotation(map.addAnnotation(LineAnnotation { LineString<double>() }));
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    test::render(map);
}

TEST(Annotations, SwitchStyle) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(map);

    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");

    checkRendering(map, "switch_style");
}

TEST(Annotations, QueryRenderedFeatures) {
    util::RunLoop loop;

    auto display = std::make_shared<mbgl::HeadlessDisplay>();
    HeadlessView view(display, 1);
    StubFileSource fileSource;

    Map map(view, fileSource, MapMode::Still);
    map.setStyleJSON(util::read_file("test/fixtures/api/empty.json"), "");
    map.addAnnotationIcon("default_marker", namedMarker("default_marker.png"));
    map.addAnnotation(SymbolAnnotation { Point<double> { 0, 0 }, "default_marker" });

    test::render(map);
    
    auto point = map.pixelForLatLng({ 0, 0 });
    auto features = map.queryRenderedFeatures(point);
    EXPECT_EQ(features.size(), 1);
}
