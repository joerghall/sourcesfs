// ExpandRepoUrl.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <unordered_map>
#include <regex>
#include <string>
using namespace std;

using path_fragment = string;

struct RepoConfig
{
    vector<string> args;
    string urlTemplate;
};

struct RepoPathInfo
{
    const RepoConfig& repoConfig;
    string url;
    vector<path_fragment> pathFragments;
};

string expandRepoUrlTemplate(
    const RepoConfig& repoConfig,
    vector<path_fragment>::const_iterator& iter,
    vector<path_fragment>::const_iterator& endIter)
{
    unordered_map<string, string> argValues;
    auto url(repoConfig.urlTemplate);
    for (const auto& arg : repoConfig.args)
    {
        const auto token = "${" + arg + "}";
        const auto& argValue = *iter++;
        const auto findIter = url.find(token);
        if (findIter != string::npos)
        {
            url.replace(findIter, token.size(), argValue);
        }
    }

    return url;
}

RepoPathInfo resolveRepoPathInfo(
    const unordered_map<string, RepoConfig>& repoConfigs,
    const vector<path_fragment>& pathFragments)
{
    if (pathFragments.size() < 1)
    {
        throw runtime_error("Invalid path");
    }

    auto pathFragmentIter = pathFragments.cbegin();
    auto pathFragmentEndIter = pathFragments.cend();

    const auto providerName = *pathFragmentIter++;
    const auto repoConfigIter = repoConfigs.find(providerName);
    if (repoConfigIter == repoConfigs.cend())
    {
        throw runtime_error("Could not find provider \"" + providerName + "\"");
    }

    const auto& repoConfig = repoConfigIter->second;
    const auto url = expandRepoUrlTemplate(repoConfig, pathFragmentIter, pathFragmentEndIter);

    return RepoPathInfo
    {
        repoConfig,
        url,
        { pathFragmentIter, pathFragmentEndIter }
    };
}

int main()
{
    unordered_map<string, RepoConfig> repoConfigs;
    repoConfigs.emplace(make_pair<string, RepoConfig>("gitlab", { { "group", "name" }, "git@gitlab.com:${group}/${name}.git" }));

    const auto repoPathInfo = resolveRepoPathInfo(repoConfigs, { "gitlab", "xxx", "yyy", "commit", "a", "b", "c" });
    cout << repoPathInfo.url << endl;
    for (const auto& pathFragment : repoPathInfo.pathFragments)
    {
        cout << "  " << pathFragment << endl;
    }

    return 0;
}
