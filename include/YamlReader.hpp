/** YamlReader.hpp generated by R. Yokoyama 09/04/2018 **/
#ifndef VANDLE_MERGER_YAMLREADER_HPP_
#define VANDLE_MERGER_YAMLREADER_HPP_

#include <iostream>
#include "YamlParameter.hpp"

class YamlReader
{
public:
    YamlReader(){}
    YamlReader(const std::string &yaml_node_name);
    ~YamlReader(){}

    YAML::Node GetNode(const std::string &attr,const bool &throw_error=true) const;
    std::string GetString(const std::string &attr, const bool &throw_error=true, const std::string &default_str="default") const;
    unsigned long long GetULong64(const std::string &attr, const bool &throw_error=true, const unsigned long long &default_val=0) const;
    double GetDouble(const std::string &attr, const bool &throw_error=true, const double &default_val=0) const;
    double GetBoolean(const std::string &attr, const bool &throw_error=true, const bool &default_val=false) const;

private:
    const static std::string kMsgPrefix;
    YAML::Node yaml_;

};

#endif /** VANDLE_MERGER_YAMLREADER_HPP_ **/
