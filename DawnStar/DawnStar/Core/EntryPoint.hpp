#pragma once

#ifdef DS_PLATFORM_WINDOWS

extern DawnStar::Application* DawnStar::CreateApplication();

int main(int argc, char** argv)
{
    DawnStar::Log::Init();

    auto app = DawnStar::CreateApplication();

    app->Run();

    delete app;
}

#endif