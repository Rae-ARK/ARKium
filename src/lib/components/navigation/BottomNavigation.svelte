<script lang="ts">
	import { page } from '$app/state';
	import { resolve } from '$app/paths';
	import { navigation } from '$lib/config/navigation';
</script>

<nav class="bottom-nav">
	{#each navigation.main as item (item.path)}
		<a href={resolve(item.path)} class:active={page.url.pathname === item.path}>
			{item.label}
		</a>
	{/each}
</nav>

<style>
	.bottom-nav {
		position: fixed;

		left: 0;
		right: 0;
		bottom: 0;

		height: 72px;

		display: flex;
		align-items: center;
		justify-content: space-evenly;

		background: var(--surface);

		border-top: 1px solid var(--border);

		z-index: 100;
	}

	.bottom-nav a {
		text-decoration: none;
		color: var(--text-secondary);

		font-size: 0.82rem;
	}

	.bottom-nav a.active {
		color: var(--accent);
		font-weight: 700;
	}
</style>
