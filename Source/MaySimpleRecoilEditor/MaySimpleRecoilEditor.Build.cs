// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MaySimpleRecoilEditor : ModuleRules
{
	public MaySimpleRecoilEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"EditorStyle",
				"UnrealEd",
				"Projects",
				"PropertyEditor",  // Notwendig für Detail Customization
				"MaySimpleRecoil"  // Verknüpft das Editor-Modul mit dem Runtime-Modul
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"InputCore",
				"EditorStyle",
				"UnrealEd",
				"Projects",
				"PropertyEditor",  // Notwendig für Detail Customization
				"MaySimpleRecoil"  // Verknüpft das Editor-Modul mit dem Runtime-Modul
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
