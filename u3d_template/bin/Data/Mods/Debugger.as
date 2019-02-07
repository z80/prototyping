/**
 * Array of all the loaded mod (script) names
 */
Array<String>@  mods = {};
Array<Text@> textElements;

/**
 * Entry function for the mod
 */
void Start()
{
    log.Info("Debugger.as START");
    SubscribeToEvent("SetLevel", "HandleLevelChange");
    SubscribeToEvent("LevelChangingFinished", "HandleLevelLoaded");
    SubscribeToEvent("ModsLoaded", "HandleModsLoaded");
    SubscribeToEvent("InputMappingFinished", "HandleInputMappingFinished");
    SubscribeToEvent("LevelChangingStarted", "HandleLevelChangingStarted");
    SubscribeToEvent("LevelChangingFinished", "HandleLevelChangingFinished");

    // Add new global config value
    VariantMap data;
    data["Name"] = "MyTestVariable";
    SendEvent("AddConfig", data);

    // Set a value for our variable
    SetGlobalVar("MyTestVariable", 123.21);

    VariantMap loadConfigData;
    loadConfigData["Filepath"] = "Data/Mods/Helpers/Config.json";
    loadConfigData["Prefix"] = "Debugger_";
    // Load custom config file
    SendEvent("LoadConfig", loadConfigData);

    log.Info("Developer name: " + GetGlobalVar("Debugger_Developer").GetString());

    DelayedExecute(5.0, true, "void ShowNotitification()");
}

void HandleLevelChangingStarted(StringHash eventType, VariantMap& eventData)
{
    String from = eventData["From"].GetString();
    String to = eventData["To"].GetString();
}

void HandleLevelChangingFinished(StringHash eventType, VariantMap& eventData)
{
    String from = eventData["From"].GetString();
    String to = eventData["To"].GetString();
}

void ShowNotitification()
{
    VariantMap data;
    data["Message"] = "Uptime seconds: " + String(time.elapsedTime);
    SendEvent("ShowNotification", data);
}

// void StopSound()
// {
//     VariantMap data;
//     data["Index"] = -1;
//     data["Type"] = "Music";
//     SendEvent("StopSound", data);
//     // DelayedExecute(1.0, false, "void PlaySound()");
// }

void Stop()
{
    log.Info("Debugger.as STOP");
}

void HandleInputMappingFinished(StringHash eventType, VariantMap& eventData)
{
    log.Info("------------");
    log.Info("Controller: " + eventData["Controller"].GetString());
    log.Info("ControlAction: " + String(eventData["ControlAction"].GetInt()));
    log.Info("ActionName: " + eventData["ActionName"].GetString());
    log.Info("Key: " + String(eventData["Key"].GetInt()));
    log.Info("KeyName: " + eventData["KeyName"].GetString());
    log.Info("------------");
}

/**
 * Output debug message when level changing is requested
 */
void HandleLevelChange(StringHash eventType, VariantMap& eventData)
{
    String levelName = eventData["Name"].GetString();
    log.Info("[Debugger.as] Level loaded: " + levelName);

    if (levelName == "Splash") {
        // Stop all previous music first
        VariantMap data;
        data["Index"] = -1;
        data["Type"] = "Music";
        SendEvent("StopSound", data);

        // Play sound by our choice
        data["Index"] = 0;
        data["Type"] = "Music";
        SendEvent("PlaySound", data);
    }
    if (levelName == "MainMenu") {
        //Stop all previous music first
        VariantMap data;
        data["Index"] = -1;
        data["Type"] = "Music";
        SendEvent("StopSound", data);

        // Play sound by our choice
        data["Index"] = 1;
        data["Type"] = "Music";
        SendEvent("PlaySound", data);
    }
}

/**
 * Display all the loaded mods(scripts) when the level is finished loading
 */
void HandleLevelLoaded(StringHash eventType, VariantMap& eventData)
{
    DrawModNames();
}

/**
 * When all the mods are loaded, retrieve the list
 */
void HandleModsLoaded(StringHash eventType, VariantMap& eventData)
{
    mods = eventData["Mods"].GetStringVector();
    log.Info("[Debugger.as] Total mods loaded: " + mods.length);
    DrawModNames();
}

/**
 * Create a list of loaded mods(scripts)
 */
void DrawModNames()
{
    if (mods.empty) {
        return;
    }

    for (uint i = 0; i < textElements.length; i++) {
        textElements[i].Remove();
    }
    textElements.Clear();
    CreateScriptName("Mods Loaded:", 0);
    for (uint i = 0; i < mods.length; i++) {
        CreateScriptName("    " + mods[i], i + 1);
    }
}

/**
 * Create a single text element
 */
void CreateScriptName(String name, int index)
{
    int fontSize = 10;
    int margin = 2;
     // Construct new Text object
    Text@ helloText = Text();

    // Set String to display
    helloText.text = name;

    // Set font and text color
    helloText.SetFont(cache.GetResource("Font", "Fonts/Muli-Regular.ttf"), fontSize);
    helloText.color = Color(0.0f, 0.8f, 0.0f);

    // Align Text center-screen
    helloText.horizontalAlignment = HA_LEFT;
    helloText.verticalAlignment = VA_TOP;

    helloText.position = IntVector2(10, 10 + index * (fontSize + margin));

    // Add Text instance to the UI root element
    ui.root.AddChild(helloText);

    textElements.Push(helloText);
}