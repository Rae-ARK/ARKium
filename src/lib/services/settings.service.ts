import type { Settings } from '$lib/models/settings';

export async function loadSettings(): Promise<Settings> {
	return {
		theme: 'system'
	};
}

export async function saveSettings(settings: Settings): Promise<void> {
	void settings;

	// Future:
	// Capacitor storage / native persistence
}
