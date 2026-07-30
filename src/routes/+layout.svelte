<script lang="ts">
	import { page } from '$app/state';
	import { resolve } from '$app/paths';
	import { navigation } from '$lib/config/navigation';
	import { settings } from '$lib/stores/settings.svelte';
	import '$lib/theme/reset.css';
	import '$lib/theme/theme.css';

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

<header>
	<img src="/icon.png" alt="ARKium" />
	<span>ARKium</span>
</header>

<nav>
	{#each navigation.main as item (item.path)}
		<a href={resolve(item.path)} class:active={page.url.pathname === item.path}>
			{item.label}
		</a>
	{/each}
</nav>

<main>
	{@render children()}
</main>

<style>
	header {
		display: flex;
		align-items: center;
		gap: 0.75rem;
		padding: 1rem;
		background: var(--surface);
		color: var(--text);
	}

	header img {
		width: 32px;
		height: 32px;
		border-radius: 8px;
	}

	nav {
		display: flex;
		align-items: center;
		gap: 1rem;
		padding: 1rem;
		background: var(--surface);
	}

	nav h2 {
		margin: 0;
		font-size: 1rem;
	}
	a {
		color: var(--text);
		text-decoration: none;
	}

	.active {
		font-weight: bold;
	}

	main {
		padding: 1rem;
	}
</style>
