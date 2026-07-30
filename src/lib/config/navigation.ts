export const navigation = {
	main: [
		{
			label: 'Library',
			path: '/'
		},
		{
			label: 'Import',
			path: '/import'
		},
		{
			label: 'Settings',
			path: '/settings'
		}
	],

	support: [
		{
			label: 'Getting Started',
			path: '/getting-started'
		},
		{
			label: 'Help',
			path: '/help'
		},
		{
			label: 'Privacy',
			path: '/privacy'
		},
		{
			label: 'Terms',
			path: '/terms'
		}
	],

	developer: [
		{
			label: 'Diagnostics',
			path: '/diagnostics'
		}
	]
} as const;
