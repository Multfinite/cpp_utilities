#ifndef UTILITIES_PUGIXML_PUGIXML_HPP
#define UTILITIES_PUGIXML_PUGIXML_HPP

#include <pugixml.hpp>
#include <map>
#include <string>

#include "../filesystem.hpp"
#include "../exceptions.hpp"

namespace Utilities::pugixml
{
    struct text_inserter_walker : pugi::xml_tree_walker
    {
        static constexpr char TypeKey[] = "type";
        static constexpr char VariableKey[] = "variable";
        static constexpr char TextTypeKey[] = "text";

        std::map<std::string, std::string> Mapping;

        text_inserter_walker(std::map<std::string, std::string> const& mapping) :
            Mapping(mapping)
        {}

        virtual bool for_each(pugi::xml_node& node)
        {
            if(auto const typeAttr = node.attribute(TypeKey))
                if(std::string(typeAttr.value()) != TextTypeKey) return true;
            if(auto const attr = node.attribute(VariableKey))
            {
                auto it = Mapping.find(attr.value());
                if(it != Mapping.end())
                {
                    auto t = node.child("tspan");
                    std::string const value = it->second;
                    t.text().set(value.c_str(), value.size());
                }
            }
            return true; // continue traversal
        }
    };

    inline pugi::xml_node find_chlid_by_id(pugi::xml_document const& doc, std::string const& id) noexcept
    {
       return doc.find_child([&id](pugi::xml_node const& x) noexcept -> bool {
            if(auto attribute = x.attribute("id"))
                return std::string(attribute.value()) == id;
            return false;
        });
    }

    inline pugi::xml_node find_node_by_id(pugi::xml_document const& doc, std::string const& id) noexcept
    {
        return doc.find_node([&id](pugi::xml_node const& x) noexcept -> bool {
            if(auto attribute = x.attribute("id"))
                return attribute.value() == id;
            return false;
        });
    }

    inline pugi::xml_document load_xml(fs::path const& filePath)
    {
        if(!fs::exists(filePath))
            throw construct_error_args_no_msg(Exceptions::file_not_found_error, filePath.string());

        pugi::xml_document doc;
        auto docPr = doc.load_file(filePath.c_str());
        if(docPr.status != pugi::xml_parse_status::status_ok)
            throw construct_error(Exceptions::parse_error, std::string("failed to parse .xml file\n") + "(" + std::to_string((int) docPr.status) + ") " + docPr.description());

        return doc;
    }
    inline pugi::xml_document load_xml(std::string const& fileName) { return load_xml(fs::path(Utilities::absolute(fileName))); }
}

#endif // UTILITIES_PUGIXML_PUGIXML_HPP
