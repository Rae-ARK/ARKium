<script lang="ts">
	import { settings } from '$lib/stores/settings.svelte';
	import '$lib/theme/reset.css';
	import '$lib/theme/theme.css';
	import TopAppBar from '$lib/components/layout/TopAppBar.svelte';
	import BottomNavigation from '$lib/components/navigation/BottomNavigation.svelte';

	let { children } = $props();

	function applyTheme() {
		const theme = settings.theme;

		if (theme === 'system') {
			const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
			document.documentElement.dataset.theme = prefersDark ? 'dark' : 'light';
		} else {
			document.documentElement.dataset.theme = theme;
		}
	}

	$effect(() => {
		applyTheme();

		const media = window.matchMedia('(prefers-color-scheme: dark)');

		const listener = () => {
			if (settings.theme === 'system') {
				applyTheme();
			}
		};

		media.addEventListener('change', listener);

		return () => media.removeEventListener('change', listener);
	});
</script>

<TopAppBar />

<main class="content">
	{@render children()}
</main>

<BottomNavigation />

<style>
	.content {
		padding: 20px;
		padding-bottom: 90px;

		min-height: calc(100vh - 64px);
	}
</style>
