// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UWGame : ModuleRules
{
	public UWGame(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core", 
			"CoreUObject", 
			"DeveloperSettings",
			"Engine",
			"EnhancedInput",
			"InputCore",
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
		// Implementacja własnej klasy UGameInstance lub UGameInstanceSubsystem ze zmiennymi i funkcjami do zapisywania punktacji uzyskanej na aktualnym poziomie,
		// Utworzenie i ustawienie warunków gry (czasu i minimalnej ilości owiec do złapania) dla 2 poziomów,
		// Implementacja przejścia do kolejnego poziomu po ukończeniu poprzedniego,
		// Implementacja mechaniki wyświetlania i łapania owcy, która jest oddzielona od stada, wraz z implementacją parametrów wartości poszczególnej owcy,
		// Implementacja zapisu i odczytu najwyższego uzyskanego wyniku (do rozwinięcia później przy scoreboardzie i UI)

	}
}
