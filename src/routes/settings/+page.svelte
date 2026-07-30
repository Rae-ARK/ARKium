<script lang="ts">
	import SettingCard from '$lib/components/SettingCard.svelte';
	import { settings } from '$lib/stores/settings.svelte';
	import type { Theme } from '$lib/models/settings';

	const themes: { id: Theme; title: string; description: string }[] = [
		{
			id: 'system',
			title: 'System',
			description: 'Follow your device theme'
		},
		{
			id: 'light',
			title: 'Light',
			description: 'Bright and clean reading environment'
		},
		{
			id: 'dark',
			title: 'Dark',
			description: 'Comfortable for low-light reading'
		},
		{
			id: 'paper',
			title: 'Warm Paper',
			description: 'A softer book-like appearance'
		}
	];
</script>

<h1>Settings</h1>

<section>
	<h2>Appearance</h2>

	{#each themes as theme (theme.id)}
		<SettingCard
			title={theme.title}
			description={theme.description}
			selected={settings.theme === theme.id}
			onclick={() => (settings.theme = theme.id)}
		/>
	{/each}
</section>

<section>
	<h2>About</h2>
	<p>ARKium</p>
</section>

<section>
	<h2>Diagnostics</h2>
	<p>System information will appear here.</p>
</section>

<style>
	section {
		margin-bottom: 2rem;
	}

	h2 {
		margin-bottom: 1rem;
	}
</style>
