#pragma once

#include "resource.hpp"
#include "embedded_resources/resource_15941497242258783328.hpp"
#include "embedded_resources/resource_2692289556126038438.hpp"
#include "embedded_resources/resource_4810432706299469547.hpp"
#include "embedded_resources/resource_16049670506556457972.hpp"
#include "embedded_resources/resource_16311497088183155912.hpp"

namespace {
class ResourceHolder {
  private:
    std::array<Resource, 5> resources{
        Resource(resource_15941497242258783328, resource_15941497242258783328_path),
        Resource(resource_2692289556126038438, resource_2692289556126038438_path),
        Resource(resource_4810432706299469547, resource_4810432706299469547_path),
        Resource(resource_16049670506556457972, resource_16049670506556457972_path),
        Resource(resource_16311497088183155912, resource_16311497088183155912_path),
    };

  public:
    auto Gather(const std::string& file) const {
        auto it = std::find_if(resources.begin(), resources.end(), [file](const auto& lhs) { return lhs.GetPath() == file; });
        if (it == resources.end()) throw std::runtime_error("Unable to detect resource with name " + file);

        return it->GetArray();
    }

    auto Gather(const char* file) const { return Gather(std::string(file)); }

    auto ListFiles() const {
        std::vector<std::string> dst{};
        dst.reserve(resources.size());
        for (auto& el : resources)
            dst.push_back(el.GetPath());

        return dst;
    }

    auto FindByFilename(const std::string& file) const {
        std::vector<Resource> dst{};
        dst.reserve(resources.size());
        std::copy_if(resources.begin(), resources.end(), std::back_inserter(dst), [file](const auto& item) {
            auto path         = item.GetPath();
            auto last_forward = path.find_last_of('\\');
            auto last_inverse = path.find_last_of('/');

            if (last_forward != std::string::npos)
                path = path.substr(last_forward + 1);
            else if (last_inverse != std::string::npos)
                path = path.substr(last_inverse + 1);
            return path == file;
        });

        return dst;
    }

    auto FindByFilename(const char* file) const { return FindByFilename(std::string(file)); }

    auto operator()(const std::string& file) const { return Gather(file); }

    auto operator()(const char* file) const { return Gather(std::string(file)); }
};
} // namespace

namespace rh {
ResourceHolder embed;
}
