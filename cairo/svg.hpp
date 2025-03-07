#ifndef RENDERER_SVG_HPP
#define RENDERER_SVG_HPP

#include <variant>

#include "../exceptions.hpp"
#include "../string.hpp"
#include "../pugixml/pugixml.hpp"
#include "../rsvg/rsvg.hpp"

#include "gfx_entry.hpp"

namespace Utilities::Cairo
{
    class SVG
    {
    public:
        using v2d = Utilities::Math::Vector2<double>;

        static constexpr char SvgRootNodeName[] = "svg";
        static constexpr char IdAttributeName[] = "id";
        static constexpr char StyleAttributeName[] = "style";

        inline static pugi::xml_node root_of(pugi::xml_document const& svgDoc)
        {
            auto root = svgDoc.find_node([](pugi::xml_node const& x) noexcept -> bool { return std::string(x.name()) == SvgRootNodeName; });
            if(!root)
                throw construct_error(Exceptions::invalid_argument_error, "No <svg> node in document! Is it SVG file?");
            return root;
        }
        inline static Utilities::Math::Vector2<double> size_of(pugi::xml_node const& svg) { return Utilities::Math::Vector2<double> { std::stod(svg.attribute("width").value()), std::stod(svg.attribute("height").value()) }; }
        inline static Utilities::Math::Vector2<double> size_of(pugi::xml_document const& svgDoc) { return size_of(root_of(svgDoc)); }

        template<typename T, typename TPreprocessor /* void p(T const& o, pugi::xml_document& d) */>
        inline static std::unique_ptr<RsvgHandle*> parse(pugi::xml_document& doc, T const& o, TPreprocessor&& p)
        {
            p(o, doc);

            std::stringstream ss;
            doc.save(ss);
            auto svg = ss.str();

            GError* pError = nullptr;
            auto rsvg = std::make_unique<RsvgHandle*>(rsvg_handle_new_from_data((guint8*) svg.data(), svg.size(), &pError));
            if(!rsvg)
                throw construct_error(Exceptions::parse_error, "failed to create RsvgHandle by processed svg data.");
            return rsvg;
        }

        inline static std::unique_ptr<RsvgHandle*> parse(pugi::xml_document const& doc)
        {
            std::stringstream ss;
            doc.save(ss);
            auto svg = ss.str();

            GError* pError = nullptr;
            auto rsvg = std::make_unique<RsvgHandle*>(rsvg_handle_new_from_data((guint8*) svg.data(), svg.size(), &pError));
            if(!rsvg)
                throw construct_error(Exceptions::parse_error, "failed to create RsvgHandle by processed svg data.");
            return rsvg;
        }

        template<typename T, typename TPreprocessor /* void p(T const& o, pugi::xml_document& d) */>
        inline static std::unique_ptr<RsvgHandle*> load(std::string const& fileName, T const& o, TPreprocessor&& p) { return parse(Utilities::pugixml::load_xml(fileName), o, p); }

        inline static std::unique_ptr<RsvgHandle*> load(std::string const& fileName) { return parse(Utilities::pugixml::load_xml(fileName)); }

        inline static Cairo::RefPtr<Cairo::ImageSurface> render(RsvgHandle* svg, Format format = FORMAT(ARGB32), double w = 0, double h = 0, double x = 0, double y = 0)
        {
            auto surface = Cairo::ImageSurface::create(format, w, h);
            auto context = Cairo::Context::create(surface);
            GError* pError = nullptr;
            auto rect = RsvgRectangle { x, y, w, h };
            if(!rsvg_handle_render_document(svg, context->cobj(), &rect, &pError))
                 throw construct_error_args(rasterization_error, std::string("Error during render SVG to ImageSurface. GError: (") + std::to_string(pError->code) + ")\n" + pError->message, pError);
            return surface;
        }
        inline static Cairo::RefPtr<Cairo::ImageSurface> render(RsvgHandle* svg, double w, double h) { return render(svg, FORMAT(ARGB32), w, h, 0, 0); }
        inline static GfxEntry& render(RsvgHandle* svg, GfxEntry& gfx, v2d size, v2d pos = v2d::Zero())
        {
            size = size.ceil();
            gfx.Image = Cairo::ImageSurface::create(FORMAT(ARGB32), VECTOR2_EXPAND(size));
            gfx.Context = Cairo::Context::create(gfx.Image);
            GError* pError = nullptr;
            auto rect = RsvgRectangle { VECTOR2_EXPAND(pos), VECTOR2_EXPAND(size) };
            if(!rsvg_handle_render_document(svg, gfx.Context->cobj(), &rect, &pError))
                 throw construct_error_args(rasterization_error, std::string("Error during render SVG to ImageSurface. GError: (") + std::to_string(pError->code) + ")\n" + pError->message, pError);
            return gfx;
        }

        using v2s = Utilities::Math::Vector2<std::string>;
        using __xml_node_transformation_variant = std::variant<
            std::string /* vectors as single property = "x.xx, y.yy" */
          , v2s /* vectors as a pair of two different fields */
        >;

        template<
              typename position_type_op /* v2d(v2d x) */
            , typename size_type_op /* v2d(v2d x) */
            , template<typename...> class position_container_type /*  */
            , template<typename...> class size_pair_container_type /*  */
            , template<typename...> class rect_container_type /*  */
        > inline static bool xml_node_transformation(pugi::xml_node& node
            , position_type_op const& position_op
            , size_type_op const& size_op
            , position_container_type<__xml_node_transformation_variant> positionAttributeNames
            , size_pair_container_type<__xml_node_transformation_variant> sizeAttributeNames
            , rect_container_type<std::string> rectAttributeNames
        ) {
            auto const opVector = [&node](std::string attributeName, std::function<v2d(v2d)> const& op) -> void
            {
                auto const setValue = [](pugi::xml_attribute& attribute, v2d value) constexpr -> void { attribute.set_value(Utilities::string_format("%f,%f", value.X, value.Y).c_str()); };
                auto attribute = node.attribute(attributeName.c_str());
                v2d v = v2d::Zero();
                if(attribute)
                {
                    auto splitten = Utilities::string_split(attribute.value(), ",");
                    if(splitten.size() != 2)
                        throw construct_error(Exceptions::bad_data_error, "Elements count isn't equals to 2.");
                    v = v2d { std::stod(splitten.front()), std::stod(splitten.back()) };
                }
                else return;

                setValue(attribute, op(v));
            };
            auto const opVectorPair = [&node](v2s const& attributePair, std::function<v2d(v2d)> const& op) constexpr -> void
            {
                auto const get = [&node](std::string const& attributeName) constexpr -> double
                {
                   if(auto attribute = node.attribute(attributeName.c_str()))
                       return std::stod(attribute.value());
                   return 0;
                };
                v2d v = op(v2d { get(attributePair.X), get(attributePair.Y) });
                for(auto& pair : { std::make_pair(attributePair.X.c_str(), v.X), std::make_pair(attributePair.Y.c_str(), v.Y) })
                    if(auto attribute = node.attribute(pair.first))
                        attribute.set_value(pair.second);
            };
            auto handleTransformationVariant = [&opVectorPair, &opVector]
                    (__xml_node_transformation_variant const& v, std::function<v2d(v2d)> const& op) constexpr -> void
            {
                if(std::holds_alternative<std::string>(v))
                    opVector(std::get<std::string>(v).c_str(), op);
                else if(std::holds_alternative<v2s>(v))
                    opVectorPair(std::get<v2s>(v), op);
            };

            for(__xml_node_transformation_variant const& v : positionAttributeNames)
                handleTransformationVariant(v, position_op);
            for(__xml_node_transformation_variant const& v : sizeAttributeNames)
                handleTransformationVariant(v, size_op);
            for(std::string const& attributeName : rectAttributeNames)
            {
                auto attribute = node.attribute(attributeName.c_str());
                v2d xy = v2d::Zero(), wh = v2d::Zero();
                auto vec = { &xy.X, &xy.Y, &wh.X, &wh.Y };
                if(attribute)
                {
                    auto const splitten = Utilities::string_split(attribute.value(), " ");
                    if(splitten.size() != 4)
                        throw construct_error(Exceptions::bad_data_error, "Elements count isn't equals to 4.");
                    auto vIt = vec.begin();
                    for(std::string const& v : splitten)
                    {
                        **vIt = std::stod(v);
                        ++vIt;
                    }
                }
                else continue;

                xy = position_op(xy);
                wh = size_op(wh);

                std::stringstream ss;
                for(auto vIt = vec.begin(); vIt != vec.end(); ++vIt)
                {
                    ss << **vIt;

                    auto nIt = vIt; ++nIt;
                    if(nIt != vec.end())
                        ss << " ";
                }
                attribute.set_value(ss.str().c_str());
            }

            // path
            if(node.name() == "path")
            {
                std::function<double(double x)> const op1 = [](double) constexpr -> double { return 1; };
                std::function<double(double x)> const opx = [&position_op](double x) constexpr -> double { return position_op(v2d { x, 0 }).X; };
                std::function<double(double y)> const opy = [&position_op](double y) constexpr -> double { return position_op(v2d { 0, y }).Y; };
                std::function<double(double x)> const opw = [&size_op](double w) constexpr -> double { return size_op(v2d { w, 0 }).X; };
                std::function<double(double y)> const oph = [&size_op](double h) constexpr -> double { return size_op(v2d { 0, h }).Y; };
                std::function<double(double x)> const opX = [&opx, &opw](double x) constexpr -> double { return opw(opx(x)); };
                std::function<double(double x)> const opY = [&opy, &oph](double y) constexpr -> double { return oph(opy(y)); };

                /* https://developer.mozilla.org/en-US/docs/Web/SVG/Attribute/d#path_commands */
                std::vector<std::function<double(double x)>> __x__ = { opx };
                std::vector<std::function<double(double x)>> __y__ = { opy };
                std::vector<std::function<double(double x)>> __xy__ = { opx, opy };
                std::vector<std::function<double(double x)>> __xyxy__ = { opx, opy, opX, opY };
                std::vector<std::function<double(double x)>> __xyxyxy__ = { opx, opy, opX, opY, opX, opY };
                std::vector<std::function<double(double x)>> __wh111xy__ = { opw, oph, op1, op1, op1, opx, opy };
                std::map<char, std::vector<std::function<double(double x)>>> fVec {
                      { 'M', __xy__ } , { 'm', __xy__ }
                    , { 'L' , __xy__ } , { 'l'  , __xy__ }
                    , { 'H' , __x__  } , { 'h' , __x__   }
                    , { 'V' , __y__  } , { 'v' , __y__   }
                    , { 'C', __xyxyxy__ }, { 'c', __xyxyxy__ }
                    , { 'S', __xyxy__ }, { 's', __xyxy__ }
                    , { 'Q', __xyxy__ }, { 'q', __xyxy__ }
                    , { 'T' , __xy__ } , { 't'  , __xy__ }
                    , { 'A' , __wh111xy__ } , { 'a'  , __wh111xy__ }
                };

                auto const isClosing = [](char c) constexpr noexcept -> bool { return c == 'Z' || c == 'z'; };

                auto attribute = node.attribute("d");
                auto splitten = Utilities::string_split(attribute.value(), " ");

                std::stringstream ss;
                for(auto it = splitten.begin(); it != splitten.end(); ++it)
                {
                    char const pathCommand = (*it)[0];

                    if(it != splitten.begin())
                        ss << " ";
                    ss << pathCommand;

                    if(isClosing((*it)[0]))
                         continue;

                    decltype(fVec)::mapped_type& fs = fVec[pathCommand]; ++it;
                    std::vector<double> v; v.reserve(fs.size());
                    for(std::string& x : Utilities::string_split(*it, ","))
                        v.push_back(std::stod(x));

                    auto ifs = fs.begin();
                    for(auto iv = v.begin(); iv != v.end(); ++iv, ++ifs)
                    {
                        ss << (*ifs)(*iv);
                        auto ivn = iv; ++ivn;
                        if(ivn != v.end())
                            ss << ",";
                    }
                    ss << " ";
                }

                attribute.set_value(ss.str().c_str());
            }
            return true; // continue traversal
        }

        /*!
         * @brief Move & scale
         */
        struct transform_walker : pugi::xml_tree_walker
        {
            v2d Translation = v2d::Zero();
            v2d Scale = v2d::Identity();

            virtual bool for_each(pugi::xml_node& node)
            {
                auto const positionOp =  [this](v2d v) noexcept -> v2d { return v2d::compound(v + Translation, Scale); };
                auto const sizeOp = [this](v2d v) noexcept -> v2d { return v2d::compound(v, Scale); };
                return xml_node_transformation(node
                    , positionOp
                    , sizeOp
                    , std::initializer_list<__xml_node_transformation_variant>{ "position", "start_point", "center_point", "end_point", v2s{ "x", "y" } }
                    , std::initializer_list<__xml_node_transformation_variant>{ v2s{ "width", "height" } }
                    , std::initializer_list<std::string>{ "viewBox" }
                );
            }
        };

        /*!
         * @brief Apply delta for size points only (don't affect positions)
         */
        struct resize_fixed_walker : pugi::xml_tree_walker
        {
            v2d Delta = v2d::Zero();

            resize_fixed_walker(v2d delta) : Delta(delta) {}

            virtual bool for_each(pugi::xml_node& node)
            {
                // TODO:
                // Нужно сделать так, чтобы path рос только вниз. Проблема в том, что путь построен по точкам.
                // Т.е. нужно двигать точки "снизу" вниз дальше, а точки "сверху" не двигать.
                // Отсюда вообще следует, что надо анализировать ВЕКТОР движения. Если вверх - не трогаем, если вниз - опускаем.
                // Придёться здесь расширать параметры у операторов xml_node_transformation

                auto const positionOp =  [](v2d v) constexpr noexcept -> v2d { return v; };
                auto const sizeOp = [this](v2d v) noexcept -> v2d { return v + Delta; };
                return xml_node_transformation(node
                    , positionOp
                    , sizeOp
                    , std::initializer_list<__xml_node_transformation_variant>{ "position", "start_point", "center_point", "end_point", v2s{ "x", "y" } }
                    , std::initializer_list<__xml_node_transformation_variant>{ v2s{ "width", "height" } }
                    , std::initializer_list<std::string>{ "viewBox" }
                );
            }
        };

        /*!
         * @brief Apply scalar for size points only (don't affect positions)
         */
        struct resize_scalar_walker : pugi::xml_tree_walker
        {
            v2d Scale = v2d::Identity();

            resize_scalar_walker(v2d scale) : Scale(scale) {}

            virtual bool for_each(pugi::xml_node& node)
            {
                auto const positionOp =  [](v2d v) constexpr noexcept -> v2d { return v; };
                auto const sizeOp = [this](v2d v) noexcept -> v2d { return v2d::compound(v, Scale); };
                return xml_node_transformation(node
                    , positionOp
                    , sizeOp
                    , std::initializer_list<__xml_node_transformation_variant>{ "position", "start_point", "center_point", "end_point", v2s{ "x", "y" } }
                    , std::initializer_list<__xml_node_transformation_variant>{ v2s{ "width", "height" } }
                    , std::initializer_list<std::string>{ "viewBox" }
                );
            }
        };

        inline static void resize(pugi::xml_document& svg, v2d deltaSize)
        {
            resize_fixed_walker w { deltaSize };
            svg.traverse(w);
        }
        inline static void scale(pugi::xml_document& svg, v2d scale)
        {
            resize_scalar_walker w { scale };
            svg.traverse(w);
        }

        template<typename style_type_op /* void style_op(std::map<std::string, std::string>& styleSheet)*/>
        inline static void edit_style(pugi::xml_node& n, style_type_op const& style_op)
        {
            if(auto attribute = n.attribute(StyleAttributeName))
            {
    #if 0
                std::cout
                                << n.parent().name() << " " << n.parent().attribute("id").value() << "->"
                                << n.name() << " " << n.attribute("id").value() << "->"
                                << attribute.name() << " " << attribute.value()
                                << std::endl;
    #endif
                auto styleSheet = Utilities::csv_parse_pairs(attribute.value(), ";", ":");
                style_op(styleSheet);
                attribute.set_value(Utilities::csv_dump_pairs(styleSheet, ";", ":").c_str());
            }
        }

        struct fill_recolor_node_tree_walker : pugi::xml_tree_walker
        {
            static constexpr char TypeKey[] = "fill";

            Color FillColor;

            fill_recolor_node_tree_walker(Color fillColor) : FillColor(fillColor)
            {}

            virtual bool for_each(pugi::xml_node& node)
            {
                edit_style(node, [this](std::map<std::string, std::string>& styleSheet) -> void
                {
                    try
                    {
                        auto value = styleSheet.at(TypeKey); value.erase(value.begin()); // value.pop_front() // '#XXXXXX' -> 'XXXXXX'
                        auto const color = Color::from_hex(value);
                        auto const recolored = Color::compound(FillColor, color);
                        auto const hex = "#" + recolored.as_hex();
                        styleSheet[TypeKey] = hex;
                    }
                    catch(std::out_of_range const& e) {} // .at(...)
                    catch(std::runtime_error const& e) {} // suppress
                });
                return true; // continue traversal
            }
        };
    private:
        optional<pugi::xml_document> _data;
        /*!
         * @brief <svg> node.
         */
        pugi::xml_node _root;
        /*!
         * @brief { svg.width, svg.height }
         */
        v2d _size = v2d::Zero();

        std::unique_ptr<RsvgHandle*> _rsvg;
    public:
        inline constexpr v2d size() const noexcept { return _size; }
        inline constexpr bool empty() const noexcept { return !_data.has_value(); }
        inline constexpr operator bool() const noexcept { return _data.has_value(); }

        inline constexpr pugi::xml_node& root() noexcept { return _root; }
        inline constexpr pugi::xml_document& data() noexcept { return _data.value(); }
        inline pugi::xml_document& emplace(decltype(_data)::value_type const& value)
        {
            _rsvg.reset();
            if(!_data) _data.emplace();
            (*_data).reset(value);
            _root = root_of(*_data);
            _size = size_of(_root);
            return *_data;
        }
        inline void reset() noexcept
        {
            _data.reset();
            _rsvg.reset();
            _size = v2d::Zero();
        }
        inline RsvgHandle* rsvg() const noexcept { return *_rsvg; }

        SVG() noexcept
        {
            reset();
        }
        SVG(pugi::xml_document const& data) noexcept
        {
            emplace(data);
        }
        SVG(SVG const& other) noexcept
        {
            if(other._data)
                emplace(*other._data);
            else
                reset();
        }
        inline SVG& operator=(SVG const& other) noexcept
        {
            if(this != &other)
            {
                if(other._data)
                    emplace(*other._data);
                else
                    reset();
            }
            return *this;
        }
        inline SVG& operator=(pugi::xml_document const& data) noexcept
        {
            emplace(data);
            return *this;
        }

        inline Cairo::RefPtr<Cairo::ImageSurface> render(Format format = FORMAT(ARGB32), double w = 0, double h = 0, double x = 0, double y = 0)
        {
            if(!_data || !(*_data))
                throw construct_error(Exceptions::invalid_state_error, "SVG data is empty/not initialized.");
            if(!_rsvg)
                _rsvg = parse(*_data);
            return render(*_rsvg, format, w, h, x, y);
        }

        inline GfxEntry& render(GfxEntry& gfx, v2d pos = v2d::Zero())
        {
            if(!_data || !(*_data))
                throw construct_error(Exceptions::invalid_state_error, "SVG data is empty/not initialized.");
            if(!_rsvg)
                _rsvg = parse(*_data);
            return render(*_rsvg, gfx, _size, pos);
        }

        inline void resize(v2d size)
        {
            if(!_data || !(*_data))
                throw construct_error(Exceptions::invalid_state_error, "SVG data is empty/not initialized.");

            _rsvg.reset();

            // X0 / Sx0 = X1 / Sx1
            // X0 / X1 = (Sx0 / Sx1) = const
            // X1 * const = X0

            auto const deltaSize = size - _size;

            resize(*_data, deltaSize);
            _size = size_of(_root);
        }

        inline void reload()
        {
            if(!_data || !(*_data))
                throw construct_error(Exceptions::invalid_state_error, "SVG data is empty/not initialized.");
            _rsvg = parse(*_data);
        }
    };
}

#endif // RENDERER_SVG_HPP
