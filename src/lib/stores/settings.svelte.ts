import type { Settings } from '$lib/models/settings';

export const settings = $state<Settings>({
	theme: 'system'
});
